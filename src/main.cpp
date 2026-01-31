#include "SIMDMatrix.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

// TODOs:
// 1. Adjacency matrix (DONE)
// 2. Invent an easy file structure to parse (DONE)
// 3. Graph parsing (DONE)
// 4. Alg for finding paths with specified length
// 5. Alg for finding if the digraph is acyclic or cyclic
// 6. Some tests maybe?

static void logError(const std::string_view content)
{
	static const std::string ERROR_STR = fmt::format(fmt::emphasis::bold, "ERROR");
	fmt::print(fmt::fg(fmt::color::red), "{}: {}\n", ERROR_STR, content);
}

class Digraph
{
	using LookupTable_t = std::unordered_map<std::string, size_t>;
	using SIMDMatrix = linear_algebra::SIMDMatrix;
public:
	Digraph()
		: m_verticesCount(0)
	{ }

	Digraph(size_t verticesCount)
		: m_verticesCount(verticesCount), m_adjMatrix(verticesCount)
	{}

	bool isLeadingTo(const std::string_view from, const std::string_view to) const
	{
		size_t fvIx, tvIx;

		if (!vertexExists(from) || !vertexExists(to))
			return false; // wrong vertex was specified

		fvIx = m_lookupTable.at(from.data());
		tvIx = m_lookupTable.at(to.data());

		auto value = m_adjMatrix.get(fvIx, tvIx);
		return value == 1.0f;
	}

	void findAllPathsWithLength(uint64_t length) const
	{
		SIMDMatrix walkMatrix = linear_algebra::pow(m_adjMatrix, length);

		size_t pathCount = 0;
		for (size_t i = 0; i < walkMatrix.getRowCount(); i++)
		for (size_t j = 0; j < walkMatrix.getColCount(); j++)
		{
			if (i == j)
				continue;

			float paths = walkMatrix.get(i, j);
			if (paths > 0.0f)
			{
				fmt::print("There are {} paths of length {} from {} to {}\n", paths, static_cast<uint32_t>(length), m_ixToVert.at(i), m_ixToVert.at(j));
				pathCount++;
			}
		}

		fmt::println("{} paths of length {} were found!", pathCount, length);
	}

	static Digraph fromFile(const std::string_view filepath)
	{
		std::ifstream file(filepath.data());

		json data;
		try
		{
			data = json::parse(file);
		}
		catch (const std::runtime_error& e)
		{
			throw std::runtime_error("File is not a valid json");
		}

		if (not data.contains("vertices") || not data.contains("edges"))
			throw std::runtime_error("Vertices or edges properties are missing");

		if (not data["vertices"].is_array() || not data["edges"].is_array())
			throw std::runtime_error("Vertices or edges should be defined as arrays of properties");

		const std::vector<std::string_view> vertices = data["vertices"];
		const std::vector<json> edges = data["edges"];

		if (vertices.size() == 0 || edges.size() == 0)
			throw std::runtime_error("A graph described in file should have at least 2 vertices and 1 edge");

		Digraph digraph(vertices.size());
		auto& mat = digraph.m_adjMatrix;
		auto& lt = digraph.m_lookupTable;

		// not the best solution in terms of complexity, but works.
		for (const auto& edge : edges)
		{
			if (not edge.contains("from") || not edge.contains("to"))
				throw std::runtime_error("Missing \"from\" or \"to\" property in edge");
			
			std::string from = edge["from"].get<std::string>();
			std::string to = edge["to"].get<std::string>();
			auto fvIter = std::find(vertices.begin(), vertices.end(), from);
			auto tvIter = std::find(vertices.begin(), vertices.end(), to);

			if (fvIter == vertices.end() || tvIter == vertices.end())
				throw std::runtime_error("Nonexistent vertex specified in an edge description");

			// basically we're figuring out which vertex in order this is
			size_t fvIx = std::distance(vertices.begin(), fvIter);
			size_t tvIx = std::distance(vertices.begin(), tvIter);

			mat.set(fvIx, tvIx, 1.0f);

			// lookup table serves for quick matrix col/row index finding,
			// so we don't need to do searches around the array of vertices
			digraph.m_ixToVert.try_emplace(fvIx, from);
			lt.try_emplace(std::move(from), fvIx);
			digraph.m_ixToVert.try_emplace(tvIx, to);
			lt.try_emplace(std::move(to), tvIx);
		}

		return digraph;
	}

private:
	bool vertexExists(const std::string_view v) const
	{
		if (m_lookupTable.find(v.data()) != m_lookupTable.end())
			return true;

		return false;
	}

private:
	size_t m_verticesCount;
	SIMDMatrix m_adjMatrix;
	LookupTable_t m_lookupTable;
	std::unordered_map<size_t, std::string> m_ixToVert;
};

int main(int argc, char** argv)
{
	argparse::ArgumentParser program("GraphMatrix", "1.0");
	program.add_argument("desc_file")
		.help("File describing a digraph (look into manual)")
		.required();

	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::exception& e)
	{
		fmt::print("{}\n", program.help().str());
		return 0;
	}

	const std::string descFilePath = program.get<std::string>("desc_file");
	if (not fs::exists(descFilePath))
	{
		logError("Specified file doesn't exist");
		return -1;
	}
	
	if (not isAVX2Supported())
	{
		logError("AVX2 is not supported on this machine. Please try another one.");
		return -1;
	}

	// parse
	Digraph graph;

	try
	{
		graph = Digraph::fromFile(descFilePath);
	}
	catch (const std::runtime_error& e)
	{
		logError(e.what());
		return -1;
	}

	bool run = true;
	while (run)
	{
		uint32_t choice;
		fmt::print("Tasks:\n\t1. Find a path of specified length\n\t2. Check if the graph is acyclic.\n\t3. exit\n\nChoice: ");
		std::cin >> choice;

		switch (choice)
		{
		case 1:
			break;
		case 2:
			break;
		case 3:
			run = false;
			break;
		}
	}

	return 0;
}