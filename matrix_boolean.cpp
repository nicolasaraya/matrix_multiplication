#include "matrix_boolean.hpp"

Matrix::Matrix(std::string path) 
{
  setFile(path);

  if (utils::validateExtension(path, "txt")) {
    binaryFormat = false;
  } else if (utils::validateExtension(path, "bin")) {
    binaryFormat = true;
  }
}

Matrix::~Matrix() 
{
  delete_csc();
  delete_csr();
}

void Matrix::setFile(std::string path) 
{
  this->path = path;
  if (path.find("delta16") != std::string::npos) {
    useDelta16 = true;
  }
}

csr_matrix* Matrix::make_csr() 
{
  if (binaryFormat) {
    return make_csr_bin();
  }

  if (csr) {
    return csr;
  }

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
  std::cout << "Nodes: " << csr->nRows() << ", Edges: " << csr->nEdges() << std::endl;
  return csr;
}

csr_matrix* Matrix::make_csr_bin()
{
  if (not binaryFormat) {
    return make_csr();
  }

  if (binaryFormat and useDelta16) {
    return make_csr_bin16();
  }
  
  if (csr) {
    return csr;
  }

  TIMERSTART(BUILD_CSR_MATRIX_BIN);
  std::ifstream file;
  file.open(path, std::ios::in | std::ios::binary);
  assert(file.is_open());

  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
      std::cout << size << " bytes readed from " << path << std::endl;
  }
  file.close();

  std::string blob(buffer.data(), buffer.size());
  std::istringstream iss(blob, std::ios::in | std::ios::binary);
  buffer.clear();

  csr = new csr_matrix();
  csr->row_ptr.push_back(0);

  uint32_t nodes;
  iss.read(reinterpret_cast<char*>(&nodes), sizeof(nodes));
  assert(nodes > 0);

  while (nodes--) {
    uint32_t id;
    iss.read(reinterpret_cast<char*>(&id), sizeof(id));

    uint32_t size;
    iss.read(reinterpret_cast<char*>(&size), sizeof(size));

    assert(size > 0);

    std::vector<uint32_t> adjacents(size); //puede ser csr->col_ind

    iss.read(reinterpret_cast<char*>(adjacents.data()),
                size * sizeof(uint32_t));

    // for (size_t i = 0; i < adjacents.size();i++) {
    //   csr->col_ind.push_back(adjacents[i]);
    // }

    csr->col_ind.insert(csr->col_ind.end(), adjacents.begin(), adjacents.end());
    csr->row_id.push_back(id);
    csr->row_ptr.push_back(csr->col_ind.size());
  }
  csr->max_row = csr->row_id.back();
  TIMERSTOP(BUILD_CSR_MATRIX_BIN);
  std::cout << "Nodes: " << csr->nRows() << ", Edges: " << csr->nEdges() << std::endl;
  return csr;
}

csr_matrix* Matrix::make_csr_bin16()
{
  if (binaryFormat and not useDelta16) {
    return make_csr_bin();
  }

  if (not binaryFormat) {
    return make_csr();
  }

  if (csr) {
    return csr;
  }

  std::cout << "Reading delta16 encoded binary file." << std::endl;
  TIMERSTART(BUILD_CSR_MATRIX_BIN16);
  std::ifstream file;
  file.open(path, std::ios::in | std::ios::binary);
  assert(file.is_open());

  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
      std::cout << size << " bytes readed from " << path << std::endl;
  }
  file.close();

  std::string blob(buffer.data(), buffer.size());
  std::istringstream iss(blob, std::ios::in | std::ios::binary);
  buffer.clear();

  csr = new csr_matrix();
  csr->row_ptr.push_back(0);

  uint32_t nodes;
  iss.read(reinterpret_cast<char*>(&nodes), sizeof(nodes));
  assert(nodes > 0);

  for (uint32_t n = 0; n < nodes; ++n) {
    uint32_t id = 0, n_adj = 0;
    iss.read(reinterpret_cast<char*>(&id), sizeof(id));
    iss.read(reinterpret_cast<char*>(&n_adj), sizeof(n_adj));

    if (n_adj == 0) continue;

    std::vector<uint32_t> adjacents;
    adjacents.reserve(n_adj);

    uint32_t prev = 0;
    iss.read(reinterpret_cast<char*>(&prev), sizeof(prev));
    adjacents.push_back(prev);

    for (uint32_t j = 1; j < n_adj; ++j) {
      uint32_t diff = 0;
      while (true) {
        uint16_t d16 = 0;
        iss.read(reinterpret_cast<char*>(&d16), sizeof(d16));
        if (d16 == 0) {
          diff += std::numeric_limits<uint16_t>::max();
        } else {
          diff += d16;
          break;
        }
      }
      prev += diff;
      adjacents.push_back(prev);
    }

    csr->col_ind.insert(csr->col_ind.end(), adjacents.begin(), adjacents.end());
    csr->row_id.push_back(id);
    csr->row_ptr.push_back(csr->col_ind.size());
  }
  csr->max_row = csr->row_id.back();
  TIMERSTOP(BUILD_CSR_MATRIX_BIN16);

  return csr;
}


csc_matrix* Matrix::make_csc() 
{
  // TODO: this depend from csr being created first. we need to fix this.

  if (csc) {
    return csc;
  }

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

void Matrix::saveTxt() const
{
  saveTxt(path);
}

void Matrix::saveTxt(std::string pathFile) const
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

void Matrix::saveBin() const
{
  saveBin(path);
}

void Matrix::saveBin(std::string pathFile) const
{
  if (useDelta16) {
    saveBin16(pathFile);
    return;
  }

  std::cout << "Saving: " << pathFile << std::endl;
  std::cout << "Edges: " << csr->nEdges() << std::endl;
  std::cout << "Nodes: " << csr->nRows() << std::endl;
  std::ofstream file(pathFile, std::ios::out | std::ios::binary | std::ofstream::trunc); 
  assert(file.is_open());

  uint32_t nodes = csr->row_id.size();
  file.write(reinterpret_cast<const char*>(&nodes), sizeof(nodes));
  
  for (size_t i = 0; i < csr->row_id.size(); i++) {
    if (csr->row_ptr[i] == csr->row_ptr[i+1]) {
      continue;
    }
    uint32_t id = csr->row_id[i];
    file.write(reinterpret_cast<const char*>(&id), sizeof(id));
    uint32_t size = csr->row_ptr[i+1] - csr->row_ptr[i];
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    file.write(reinterpret_cast<const char*>(csr->colIndPtr() + csr->row_ptr[i]),
                size * sizeof(uint32_t));
  }
  file.close();
}

void Matrix::saveBin16() const
{
  saveBin16(path);
}

void Matrix::saveBin16(std::string pathFile) const
{
  std::cout << "Saving (delta16): " << pathFile << std::endl;
  std::cout << "Edges: " << csr->nEdges() << std::endl;
  std::cout << "Nodes: " << csr->nRows() << std::endl;
  std::ofstream file(pathFile, std::ios::out | std::ios::binary | std::ofstream::trunc); 
  assert(file.is_open());

  uint32_t nodes = csr->row_id.size();
  file.write(reinterpret_cast<const char*>(&nodes), sizeof(nodes));
  
  for (size_t i = 0; i < csr->row_id.size(); i++) {
    uint32_t id = csr->row_id[i];
    if (csr->row_ptr[i] == csr->row_ptr[i+1]) {
      continue;
    }
    file.write(reinterpret_cast<const char*>(&id), sizeof(id));
    uint32_t size = csr->row_ptr[i+1] - csr->row_ptr[i];
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (size == 0) continue;

    uint32_t prev = csr->col_ind[csr->row_ptr[i]];
    file.write(reinterpret_cast<const char*>(&prev), sizeof(prev));

    for (size_t j = 1; j < size; ++j) {
      uint32_t curr = csr->col_ind[csr->row_ptr[i] + j];
      uint32_t diff = curr - prev;
      uint32_t rest = diff;
      while (rest > std::numeric_limits<uint16_t>::max()) {
        uint16_t zero = 0;
        file.write(reinterpret_cast<const char*>(&zero), sizeof(zero));
        rest -= std::numeric_limits<uint16_t>::max();
      }
      uint16_t d16 = static_cast<uint16_t>(rest);
      file.write(reinterpret_cast<const char*>(&d16), sizeof(d16));
      prev = curr;
    }
  }
  file.close();
}
