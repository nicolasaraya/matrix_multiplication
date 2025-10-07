import os
import subprocess
import tarfile

# -------------------------------
# params
# -------------------------------
opt = 1  # activar optimización
delta16 = 0  # usar delta16
showOutput = False  # mostrar salida en pantalla
runAdrian = True  # ejecutar pasos Adrian

fullRun = True

# -------------------------------
# Configuración
# -------------------------------
workdir = "/home/naraya2018"
base_path = f"{workdir}/datasets"

# -------------------------------
# Ejecutables
# -------------------------------
biclique_extractor = ["/usr/bin/time", "-v", f"{workdir}/biclique_extraction/biclique_extractor"]
#matrix_multiplicator = ["/usr/bin/time", "-v", f"{workdir}/matrix_multiplication/matrix_multiplicator"]
ng = ["/usr/bin/time", "-v", f"{workdir}/ng_multiplication/ng"]
merged = ["/usr/bin/time", "-v", f"{workdir}/ng_multiplication/merged"]
pow4_exec = ["/usr/bin/time", "-v", f"{workdir}/ng_multiplication/pow4"]

# Adrian tools
adrian_format = f"{workdir}/zagal-mem/zagal/matrix_multiplication/adrian_format"
b_format = f"{workdir}/zagal-mem/zagal/matrix_multiplication/b_format"
adrian_multi = ["/usr/bin/time", "-v", f"{workdir}/zagal-mem/zagal/matrix_multiplication/adrian_multi"]
adrian_multi_b = ["/usr/bin/time", "-v", f"{workdir}/zagal-mem/zagal/matrix_multiplication/adrian_multi_b"]
compress = f"{workdir}/zagal-mem/zagal/transform_bic/compresss"

# Lista de datasets a procesar
datasets = [
#    "dblp-2011",
#    "cnr-2000-hc",
#    "web-Stanford",
#    "wikipedia_link_lmo",
#    "web-Google"
#    "indochina-2004"
    "arabic-2005-hc"
]

subFixAdrian = ".pow.mat"
subFixAdrianB = ".pow_b.mat"
log_prefix = ""

# -------------------------------
# Funciones auxiliares
# -------------------------------

def run_command(cmd, logfile=None):
    """
    Ejecuta un comando y opcionalmente guarda la salida en un log.
    
    Args:
        cmd (list): Comando y argumentos.
        logfile (str, optional): Ruta del archivo de log. Si None, no guarda en archivo.
    """
    print(f"Ejecutando: {' '.join(cmd)}")
    print(f"Log: {logfile if logfile else 'No se guarda log'}")
    
    if showOutput:
        # Mostrar en tiempo real y opcionalmente guardar
        if logfile:
            f = open(logfile, "w")
        else:
            f = None

        try:
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
            for line in p.stdout:
                print(line, end="")        # imprime en pantalla
                if f:
                    f.write(line)          # guarda en el log
            p.wait()
            if p.returncode != 0:
                raise subprocess.CalledProcessError(p.returncode, cmd)
        finally:
            if f:
                f.close()
    else:
        # Solo guardar en log, silencioso
        if logfile:
            with open(logfile, "w") as f:
                subprocess.run(cmd, stdout=f, stderr=subprocess.STDOUT, check=True)
        else:
            # Ninguna salida, solo ejecutar
            subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT, check=True)


def appendsize(path, label, sizes_file):
    """Agrega el tamaño de un archivo al registro."""

    if os.path.exists(path):
        size = os.path.getsize(path)
        with open(sizes_file, "a") as f:
            f.write(f"{label}: {size}\n")

# -------------------------------
# Funciones genéricas
# -------------------------------

def run_extraction(input_file, log_prefix):
    """
    Ejecuta biclique_extractor y registra tamaños de archivos de salida.
    
    Retorna la lista de archivos de salida existentes.
    """
    print(f"\n--- Extracción: {input_file} ---")
    dataset_path = os.path.dirname(input_file)
    logfile = os.path.join(dataset_path, f"{log_prefix}.log")

    # Construir comando
    cmd = biclique_extractor + ["--file", input_file]
    if opt == 1:
        cmd += ["--optimize", "1"]

    if delta16 == 1:
        cmd += ["--useDelta", "1"]

    # Ejecutar comando
    run_command(cmd, logfile)

    # Archivos de salida esperados
    base_name = os.path.splitext(os.path.basename(input_file))[0]
    
    prefix = ""
    if delta16 == 1:
        prefix = "_delta16"

    expected_outputs = [
        f"{base_name}{prefix}.bin", #
        f"{base_name}_compressed.txt", # adrian
        f"{base_name}_bicliques.txt", # adrian
        f"{base_name}{prefix}_compressed.bin",
        f"{base_name}_bicliques.bin"
    ]

    existing_outputs = []
    sizes_file = os.path.join(dataset_path, f"{log_prefix}sizes.txt")
    print("\nArchivos de salida generados:")
    for fname in expected_outputs:
        fpath = os.path.join(dataset_path, fname)
        if os.path.exists(fpath):
            print(f"  {fname}")
            existing_outputs.append(fpath)
            # Registrar tamaño
            appendsize(fpath, fname, sizes_file)

    return existing_outputs


'''
def run_matrix_multiplication(input_file, log_prefix):
    print(f"\n--- Multiplicación: {input_file} ---")
    dataset_path = os.path.dirname(input_file)
    logfile = os.path.join(dataset_path, f"{log_prefix}.log")
    run_command(matrix_multiplicator + [input_file], logfile)

    if delta16 == 1:
        base_name += "_delta16"

    expected_output = os.path.join(dataset_path, f"{base_name}_pow.bin")

    sizes_file = os.path.join(dataset_path, f"{log_prefix}sizes.txt")

    if os.path.exists(expected_output):
        print(f"\nArchivo de salida generado: {os.path.basename(expected_output)}")
        appendsize(expected_output, os.path.basename(expected_output), sizes_file)
        return expected_output
    return None
'''

def run_ng(input_file, bicliques_file = None, log_prefix=""):
    print(f"\n--- NG Multiplicación: {input_file} ---")
    if bicliques_file:
        print(f"  Usando bicliques: {bicliques_file}")
        ng_cmd = ng + [input_file, bicliques_file]
    else:
        ng_cmd = ng + [input_file]

    dataset_path = os.path.dirname(input_file)
    logfile = os.path.join(dataset_path, f"{log_prefix}.log")
    run_command(ng_cmd, logfile)
    
    # Archivos de salida esperados
    base_name = os.path.splitext(os.path.basename(input_file))[0]

    

    # if delta16 == 1:
    #     base_name += "_delta16"

    expected_outputs = []
    if (bicliques_file == None):
        expected_outputs.append(os.path.join(dataset_path, f"{base_name}_pow.bin"))
    else:
      base_nameBic = os.path.splitext(os.path.basename(bicliques_file))[0]

      expected_outputs = [
          os.path.join(dataset_path, f"{base_name}_powBic_cm.bin"),
          os.path.join(dataset_path, f"{base_name}_powBic.bin"),
          os.path.join(dataset_path, f"{base_nameBic}_powBic_cb.bin")
      ]


    sizes_file = os.path.join(dataset_path, f"{log_prefix}sizes.txt")
    # Filtrar solo los existentes
    existing_outputs = [f for f in expected_outputs if os.path.exists(f)]
    for i in expected_outputs:
        print(i)
    assert len(existing_outputs) > 0, "No se generaron archivos de salida."
    print("\nArchivos de salida generados:")
    for f in existing_outputs:
        print(f"  {os.path.basename(f)}")
        # Registrar tamaño
        appendsize(f, os.path.basename(f), sizes_file)
        
    return existing_outputs



def run_merged(input_file, bicliques_file, log_prefix):
    print(f"\n--- Merged Multiplicación: {input_file} con {bicliques_file} ---")
    dataset_path = os.path.dirname(input_file)
    logfile = os.path.join(dataset_path, f"{log_prefix}.log")
    run_command(merged + [input_file, bicliques_file], logfile)

    # if delta16 == 1:
    #     base_name += "_delta16"

    base_name = os.path.splitext(os.path.basename(input_file))[0]
    expected_output = os.path.join(dataset_path, f"{base_name}_powBic.bin")
    print(expected_output)

    sizes_file = os.path.join(dataset_path, f"{log_prefix}sizes.txt")

    if os.path.exists(expected_output):
        print(f"\nArchivo de salida generado: {os.path.basename(expected_output)}")
        appendsize(expected_output, os.path.basename(expected_output), sizes_file)
        return expected_output
    return None


# -------------------------------
# Adrian-specific
# -------------------------------

def run_adrian_formats(dataset):
    """
    Ejecuta las transformaciones Adrian sobre un dataset y retorna los archivos creados.
    
    Args:
        dataset (str): Nombre del dataset (carpeta dentro de base_path)
    
    Returns:
        list: Rutas completas de archivos generados existentes
    """
    print(f"\n--- Formatos Adrian para: {dataset} ---")
    dataset_path = os.path.join(base_path, dataset)
    sizes_file = os.path.join(dataset_path, "adrianFormat_sizes.txt")
    
    if os.path.exists(sizes_file):
        os.remove(sizes_file) # Reiniciar archivo de tamaños

    # Ejecutar comandos Adrian
    run_command([adrian_format, f"{dataset_path}/{dataset}"], 
                os.path.join(dataset_path, "adrianFormat_base.log"))
    run_command([adrian_format, f"{dataset_path}/{dataset}_compressed"], 
                os.path.join(dataset_path, "adrianFormat_compressed.log"))
    run_command([compress, f"{dataset_path}/", f"{dataset}_bicliques.txt"], 
                os.path.join(dataset_path, "adrianFormat_compress.log"))
    run_command([b_format, f"{dataset_path}/"], 
                os.path.join(dataset_path, "adrianFormat_bformat.log"))

    # Lista de archivos a registrar y retornar
    files_to_check = [
        f"{dataset_path}/{dataset}.mat",
        f"{dataset_path}/{dataset}_compressed.mat",
        f"{dataset_path}/bicliques.bic",
        f"{dataset_path}/S.bin",
        f"{dataset_path}/Sbm.bin",
        f"{dataset_path}/C.bin",
        f"{dataset_path}/Cbm.bin"
    ]

    existing_files = []
    for fpath in files_to_check:
        if os.path.exists(fpath):
            existing_files.append(fpath)
            appendsize(fpath, os.path.basename(fpath), sizes_file)
            print(f"  Archivo registrado: {os.path.basename(fpath)}")

    return existing_files


def run_adrian_multiplication(input_file, output_prefix, log_file):
    """
    Ejecuta la multiplicación Adrian y retorna el archivo generado.
    
    Args:
        input_file (str): Archivo de entrada (.mat)
        output_prefix (str): Prefijo del archivo de salida
        log_prefix (str): Prefijo para el log y sizes
    
    Returns:
        str: Ruta del archivo de salida generado
    """
    print(f"\n--- Multiplicación Adrian: {input_file} ---")
    dataset_path = os.path.dirname(input_file)
    logfile = os.path.join(dataset_path, f"{log_file}.log")

    # Ejecutar comando
    run_command(adrian_multi + [input_file, output_prefix], logfile)

    # Archivo generado
    output_file = f"{output_prefix}{subFixAdrian}"

    # Registrar tamaño si existe
    sizes_file = os.path.join(dataset_path, "adrianFormat_sizes.txt")
    if os.path.exists(output_file):
        appendsize(output_file, os.path.basename(output_file), sizes_file)
        print(f"  Archivo de salida: {os.path.basename(output_file)}")
        return output_file
    return None



def run_adrian_multiplication_b(input_file, bicliques_file, output_prefix, log_file):
    """
    Ejecuta la multiplicación Adrian usando bicliques y retorna el archivo generado.
    
    Args:
        input_file (str): Archivo de entrada (.mat)
        bicliques_file (str): Archivo de bicliques (.bic)
        output_prefix (str): Prefijo del archivo de salida
        log_prefix (str): Prefijo para el log y sizes
    
    Returns:
        str: Ruta del archivo de salida generado
    """
    print(f"\n--- Multiplicación Adrian con bicliques: {input_file} y {bicliques_file} ---")
    dataset_path = os.path.dirname(input_file)
    logfile = os.path.join(dataset_path, f"{log_file}.log")

    # Ejecutar comando
    run_command(adrian_multi_b + [input_file, bicliques_file, output_prefix], logfile)

    # Archivo generado
    output_file = f"{output_prefix}{subFixAdrian}"

    # Registrar tamaño si existe
    sizes_file = os.path.join(dataset_path, "adrianFormat_sizes.txt")
    if os.path.exists(output_file):
        appendsize(output_file, os.path.basename(output_file), sizes_file)
        print(f"Archivo registrado: {os.path.basename(output_file)}")
        return output_file
    return None


# -------------------------------
# Pipeline por dataset
# -------------------------------

def process_dataset(dataset, optimize=0, log_prefix=""):
    dataset_path = os.path.join(base_path, dataset)

    # Paso 1: extracción inicial
    input1 = f"{dataset_path}/{dataset}.txt"


    outputFiles = run_extraction(input1, f"{log_prefix}extraction1")
    '''
    outputFiles contiene:
    0: .bin
    1: s_compressed.txt
    2: _bicliques.txt
    3: _compressed.bin
    4: _bicliques.bin
    '''

    # Paso 2: formatos adrian
    if runAdrian == True:
      outputAdrian = run_adrian_formats(dataset)
      '''
      outputAdrian contiene:
      0: .mat
      1: _compressed.mat
      2: .bic
      3: S.bin
      4: Sbm.bin
      5: C.bin
      6: Cbm.bin
      '''

    # -------------------------------
    # Multiplicacion baseline
    # -------------------------------
    outBaseline = run_ng(outputFiles[0], bicliques_file=None, log_prefix=f"{log_prefix}pow2_baseline")
    '''
    outBaseline contiene:
    _pow.bin
    '''
    if runAdrian == True:
      outAdrian = run_adrian_multiplication(outputAdrian[0], f"{dataset_path}/{dataset}_output", f"{log_prefix}pow2_baseline_adrian")
      '''
      outAdrian contiene:
      _pow.mat
      '''
    
    # -------------------------------
    # Multiplicacion con bicliques
    # -------------------------------
    outputMerged = run_merged(outputFiles[3], outputFiles[4], f"{log_prefix}pow2_merged")
    '''
    outputMerged contiene:
    _powBic.bin
    '''
    outputNg = run_ng(outputFiles[3], outputFiles[4], f"{log_prefix}pow2_components")
    '''
    outputNg contiene:
    _powBic_cm.bin
    _powBic_cb.bin
    _powBic.bin
    '''
    if runAdrian == True:
      outputAdrianB = run_adrian_multiplication_b(outputAdrian[1],
                                outputAdrian[2],
                                f"{dataset_path}/{dataset}_output",
                                "pow2_components_adrian")
       

    # -------------------------------
    # Segunda extracción
    # -------------------------------
    print(outBaseline)
    run_extraction(outBaseline[0], f"{log_prefix}extraction2")

    # Comprimir resultados
    
    for ext in (".bin", ".mat", ".bic", ".pairs"):
        for file in os.listdir(dataset_path):
            if file.endswith(ext):
                file_path = os.path.join(dataset_path, file)
                archive_path = os.path.join(dataset_path, f"{file}.tar.gz")
                with tarfile.open(archive_path, "w:gz") as tar:
                    tar.add(file_path, arcname=file)
                print(f"✅ Comprimido: {archive_path}")
                os.remove(file_path)



# -------------------------------
# Main loop
# -------------------------------
if __name__ == "__main__":
    print("Iniciando procesamiento de datasets...")
    print(f"Directorio base: {base_path}")
    print(f"Mostrar salida: {'Sí' if showOutput else 'No'}")
    
    if fullRun == False:
      if opt == 1:
        log_prefix = "opt_"

      print(f"=== Procesando optimización: {opt}, delta16: {delta16} ===")
      for dataset in datasets:
          print(f"=== Procesando {dataset} ===")
          try:
              process_dataset(dataset, opt, log_prefix)
          except subprocess.CalledProcessError as e:
              print(f"[ERROR] Falló el dataset {dataset}: {e}")

    else:
      # print("\n=== Ejecución completa ===")
      # opt = 0
      # delta16 = 0
      # runAdrian = True
      # log_prefix = "base_"
      # print("=== Procesando baseline ===")
      # for dataset in datasets:
      #     print(f"=== Procesando {dataset} ===")
      #     try:
      #         process_dataset(dataset, 0, log_prefix)
      #     except subprocess.CalledProcessError as e:
      #         print(f"[ERROR] Falló el dataset {dataset}: {e}")

      # print("=== Procesando optimización ===")
      # runAdrian = False
      # opt = 1
      # delta16 = 0
      # log_prefix = "opt_"
      # for dataset in datasets:
      #     print(f"=== Procesando {dataset} ===")
      #     try:
      #       process_dataset(dataset, 1, log_prefix)
      #     except subprocess.CalledProcessError as e:
      #       print(f"[ERROR] Falló el dataset {dataset}: {e}")

      print("=== Procesando delta16: ===")
      runAdrian = 0
      delta16 = 1
      opt = 0
      log_prefix = "d16_"
      for dataset in datasets:
          print(f"=== Procesando {dataset} ===")
          try:
              process_dataset(dataset, 0, log_prefix)
          except subprocess.CalledProcessError as e:
              print(f"[ERROR] Falló el dataset {dataset}: {e}")
      
      print("=== Procesando optimización + delta16 ===")
      runAdrian = 0
      delta16 = 1
      opt = 1
      log_prefix = "opt_d16"
      for dataset in datasets:
          print(f"=== Procesando {dataset} ===")
          try:
            process_dataset(dataset, 1, log_prefix)
          except subprocess.CalledProcessError as e:
            print(f"[ERROR] Falló el dataset {dataset}: {e}")
