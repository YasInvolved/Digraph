namespace fs = std::filesystem;

// TODOs:
// 1. Adjacency matrix (DONE)
// 2. Invent an easy file structure to parse
// 3. Alg for finding paths with specified length
// 4. Alg for finding if the digraph is acyclic or cyclic
// 5. Some tests maybe?

class DiGraph
{
	size_t m_vertices;
	
	std::vector<std::vector<int>> m_matrix; 

	DiGraph(size_t vertices)
		: m_vertices(vertices)
	{
		m_matrix.resize(m_vertices, std::vector<int>(m_vertices, 0));
	}

	void addEdge(int u, int v)
	{
		m_matrix[u][v] = 1;
	}
};

static bool parse_file(const std::string_view path)
{
	if (not fs::exists(path))
		return false;

	std::ifstream file(path.data());

	// TODO: parse a digraph

	return true;
}

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
		fmt::print("{}\n", program.print_help());
		return 0;
	}
	
	const std::string descFilePath = program.get<std::string>("desc_file");
	parse_file(descFilePath);

	return 0;
}