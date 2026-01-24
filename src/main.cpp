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

class Matrix
{
public:
	Matrix()
		: m_width(0), m_height(0), m_array(nullptr)
	{}

	Matrix(size_t width, size_t height)
		: m_width(width),
		m_height(height), 
		m_array(new uint32_t[width * height])
	{ }

	Matrix(size_t size)
		: m_width(size), 
		m_height(size), 
		m_array(new uint32_t[size * size])
	{ }

	~Matrix()
	{
		delete[] m_array;
	}

	void set(size_t row, size_t col, uint32_t value)
	{
		assert(m_array != nullptr); // using an unitialized matrix
		assert(row <= m_width && col <= m_height);
		m_array[row * m_width + col] = value;
	}

	uint32_t get(size_t row, size_t col)
	{
		assert(m_array != nullptr); // using an unitialized matrix
		assert(row <= m_width && col <= m_height);
		return m_array[row * m_width + col];
	}

	// TODO: + and * operators

private:
	size_t m_width, m_height;
	uint32_t* m_array;
};

class Digraph
{
public:
	Digraph()
		: m_verticesCount(0)
	{ }

	Digraph(size_t verticesCount)
		: m_verticesCount(verticesCount), m_adjMatrix(verticesCount, verticesCount)
	{}

	Matrix& getAdjMatrix()
	{
		return m_adjMatrix;
	}

	static Digraph fromFile(const std::string_view filepath)
	{
		auto parsingError = [&](const std::string_view cause)
			{
				logError(fmt::format("Failed to parse {}: {}", filepath, cause));
				std::exit(-1);
			};

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
		auto& mat = digraph.getAdjMatrix();

		// not the best solution in terms of complexity, but works.
		for (const auto& edge : edges)
		{
			if (not edge.contains("from") || not edge.contains("to"))
				throw std::runtime_error("Missing \"from\" or \"to\" property in edge");
			
			auto fvIter = std::find(vertices.begin(), vertices.end(), edge["from"].get<std::string_view>());
			auto tvIter = std::find(vertices.begin(), vertices.end(), edge["to"].get<std::string_view>());

			if (fvIter == vertices.end() || tvIter == vertices.end())
				throw std::runtime_error("Nonexistent vertex specified in an edge description");

			size_t fvIx = std::distance(vertices.begin(), fvIter);
			size_t tvIx = std::distance(vertices.begin(), tvIter);

			mat.set(fvIx, tvIx, 1);
		}

		return digraph;
	}

private:
	size_t m_verticesCount;
	Matrix m_adjMatrix;
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

	// parse
	Digraph graph;

	try
	{
		auto graph = Digraph::fromFile(descFilePath);
	}
	catch (const std::runtime_error& e)
	{
		logError(e.what());
		return -1;
	}

	return 0;
}