#include "matrix_boolean.hpp"

Matrix::Matrix(std::string path) : path(path) 
{
  if (utils::validateExtension(path, "txt")) {
    make_csr();
  } else if (utils::validateExtension(path, "bin")) {
    make_csr_bin();
  }
  make_csc();
}

Matrix::~Matrix() 
{
  delete_csc();
  delete_csr();
}

void Matrix::setFile(std::string path) 
{
  this->path = path;
}

csr_matrix* Matrix::make_csr() 
{
  TIMERSTART(BUILD_CSR_MATRIX);
  std::ifstream file;
  file.open(path);
  assert(file.is_open());
  std::string line;
  uint32_t id;
  getline(file, line); // num nodes
  uint32_t num_nodes = atoi(line.c_str());

  csr = new csr_matrix();
  csr->row_ptr.push_back(0);
  //csr->col_ind.resize(num_nodes, 0);

  while (!file.eof()) {
    getline(file, line);
    auto adjacents = utils::splitString(line, " ");

    if (adjacents.size() <= 1) {
      continue;
    }
    
    id = atoi(adjacents.at(0).c_str());

    for (size_t i = 1; i < adjacents.size();i++) {
      csr->col_ind.push_back(atoll(adjacents[i].c_str()));
    }
    csr->row_id.push_back(id);
    csr->row_ptr.push_back(csr->col_ind.size());
  }
  csr->max_row = csr->row_id.back();

  csr->col_ind.shrink_to_fit();
  csr->row_id.shrink_to_fit();
  csr->row_ptr.shrink_to_fit();

  file.close();
  TIMERSTOP(BUILD_CSR_MATRIX);

  return csr;
}

csr_matrix* Matrix::make_csr_bin()
{
  TIMERSTART(BUILD_CSR_MATRIX_BIN);
  std::ifstream file;
  file.open(path, std::ios::binary);
  assert(file.is_open());
  csr = new csr_matrix();
  csr->row_ptr.push_back(0);

  
  while (file.peek() != EOF) {
    uint32_t id;
    file.read(reinterpret_cast<char*>(&id), sizeof(id));

    uint32_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    assert(size);

    std::vector<uint32_t> adjacents(size); //puede ser csr->col_ind

    file.read(reinterpret_cast<char*>(adjacents.data()),
                size * sizeof(uint32_t));

    for (size_t i = 0; i < adjacents.size();i++) {
      csr->col_ind.push_back(adjacents[i]);
    }
    csr->row_id.push_back(id);
    csr->row_ptr.push_back(csr->col_ind.size());
  }
  csr->max_row = csr->row_id.back();
  file.close();
  TIMERSTOP(BUILD_CSR_MATRIX_BIN);

  return csr;
}

csc_matrix* Matrix::make_csc() 
{
  TIMERSTART(BUILD_CSC_MATRIX);
  csc = new csc_matrix();
  csc->col_ptr.push_back(0);
  csc->row_ind.resize(csr->col_ind.size(), 0);

  std::unordered_map<uint32_t, uint32_t> cols;

  for (size_t i = 0; i < csr->col_ind.size(); i++) {
    cols[csr->col_ind[i]]++;
  }

  for (size_t i = 0; i < cols.size(); i++) {
    if (cols[i] > 0) {
      csc->col_id.push_back(i);
      uint32_t temp = csc->col_ptr.back();
      csc->col_ptr.push_back(csc->col_ptr.back() + cols[i]);
      cols[i] = temp;
    }
  }

  for (size_t i = 0; i < csr->row_id.size(); i++) {
    size_t start = csr->row_ptr[i];
    size_t stop = csr->row_ptr[i+1];

    for (size_t j = start; j < stop; j++) {
      csc->row_ind[cols[csr->col_ind[j]]] = csr->row_id[i];
      cols[csr->col_ind[j]]++;
    }
  }

  cols.clear();

  csc->max_col = csc->col_id.back();

  csc->col_ptr.shrink_to_fit();
  csc->col_id.shrink_to_fit();
  csc->row_ind.shrink_to_fit();

  TIMERSTOP(BUILD_CSC_MATRIX);
  return csc;
}

csr_matrix* Matrix::get_csr()
{
  return csr;
}

csc_matrix* Matrix::get_csc()
{
  return csc;
}

void Matrix::set_csr(csr_matrix* mat) 
{
  this->csr = mat;
}

void Matrix::set_csc(csc_matrix* mat)
{
  this->csc = mat; 
}

void Matrix::delete_csr()
{
  if (csr) {
    delete csr;
    csr = nullptr;
  } 
}

void Matrix::delete_csc()
{
  if (csc) {
    delete csc;
    csc = nullptr;
  }
}

void Matrix::saveTxt()
{
  saveTxt(path);
}

void Matrix::saveTxt(std::string pathFile)
{
  std::cout << "Saving: " << pathFile << std::endl;
  std::cout << "Edges: " << csr->nEdges() << std::endl;
  std::cout << "Nodes: " << csr->nRows() << std::endl;
  std::ofstream file;
  file.open(pathFile, std::ofstream::out | std::ofstream::trunc); // limpia el contenido del fichero
  file << csr->col_ind.size() << std::endl;
  for (size_t i = 0; i < csr->row_id.size(); i++) {
    size_t start = csr->row_ptr[i];
    size_t stop = csr->row_ptr[i+1];

    if (start == stop) {
      continue;
    }
    
    file << csr->row_id[i] << ":";
    while (start != stop) {
      file << " " << csr->col_ind[start];
      start++;
    }
    file << std::endl;
  }
  file.close();
}

