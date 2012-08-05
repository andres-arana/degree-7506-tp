# Escaneo de archivos de fuentes y dependencias
ALL_SOURCES := $(shell find ../source/ -regex .*\.cpp$$)
ALL_DEPENDENCIES := $(shell find ./obj/ -regex .*\.d$$)
ALL_DIRECTORIES := $(shell find ../source -type d \! -path '*.svn*')
ALL_TARGET_DIRECTORIES := $(subst ../source,./obj, $(ALL_DIRECTORIES))

# Definición de archivos binarios de ejecutables
INTEGRATION_MAIN_OBJ := ./obj/integration_tests_main.o
UNIT_MAIN_OBJ := ./obj/unit_tests_main.o
HASH_MAIN_OBJ := ./obj/ppmc_hash_main.o
BTREE_MAIN_OBJ := ./obj/ppmc_btree_main.o

# Definición de archivos binarios pre-linkeo
ALL_OBJS := $(subst ../source/,./obj/,$(ALL_SOURCES:.cpp=.o))
ALL_MAIN_OBJS := $(BTREE_MAIN_OBJ) $(HASH_MAIN_OBJ) $(INTEGRATION_MAIN_OBJ) $(UNIT_MAIN_OBJ)
ALL_SHARED_OBJS := $(filter-out $(ALL_MAIN_OBJS),$(ALL_OBJS))

# Definición de target default que buildea todo el programa
all : build-btree build-hash build-unit-tests build-integration-tests

# Definición de target que linkea los exes individuales del proyecto
build-btree : build
	@echo ' '
	@echo ' '
	@echo '*******************************************************'
	@echo 'Building ppmc-btree executable'
	@echo '*******************************************************'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o"ppmct" $(ALL_SHARED_OBJS) $(BTREE_MAIN_OBJ)
	@echo 'Finished building PPMC-btree executable'
	@echo ' '
	@echo ' '
	
build-hash : build
	@echo ' '
	@echo ' '
	@echo '*******************************************************'
	@echo 'Building ppmc-hash executable'
	@echo '*******************************************************'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o"ppmch" $(ALL_SHARED_OBJS) $(HASH_MAIN_OBJ)
	@echo 'Finished building ppmc-hash executable'
	@echo ' '
	@echo ' '
	
build-unit-tests : build
	@echo ' '
	@echo ' '
	@echo '*******************************************************'
	@echo 'Building unit-tests executable'
	@echo '*******************************************************'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o"unit-tests" $(ALL_SHARED_OBJS) $(UNIT_MAIN_OBJ)
	@echo 'Finished building unit-tests executable'
	@echo ' '
	@echo ' '
	
build-integration-tests : build
	@echo ' '
	@echo ' '
	@echo '*******************************************************'
	@echo 'Building integration-tests executable'
	@echo '*******************************************************'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o"integration-tests" $(ALL_SHARED_OBJS) $(INTEGRATION_MAIN_OBJ)
	@echo 'Finished building integration-tests executable'
	@echo ' '
	@echo ' '
	
# Target que ejecutan los tests
unit-tests : build-unit-tests
	@echo ' '
	@echo ' '
	@echo '*******************************************************'
	@echo 'Running unit tests'
	@echo '*******************************************************'
	- ./unit-tests
	@echo ' '
	@echo ' '
	
integration-tests : build-integration-tests
	@echo ' '
	@echo ' '
	@echo '*******************************************************'
	@echo 'Running integration tests'
	@echo '*******************************************************'
	- ./integration-tests
	@echo ' '
	@echo ' '
	
all-tests : unit-tests integration-tests

# Target de limpieza
clean:
	@echo ' '
	@echo ' '
	@echo '*******************************************************'
	@echo 'Cleaning up'
	@echo '*******************************************************' 
	rm -rf obj logs ppmct ppmch unit-tests integration-tests
	@echo ' '
	@echo ' '

# Target de export
exporting:
	@echo ' '
	@echo ' '
	@echo '*******************************************************'
	@echo 'Exporting svn to ../entrega-tp2'
	@echo '*******************************************************' 
	-svn export .. entrega-tp2
	@echo '*******************************************************'
	@echo 'Taring GZ entrega-tp2'
	@echo '*******************************************************' 
	-tar cvfz entrega-tp2.tar.gz entrega-tp2
	-rm -rf entrega-tp2
	@echo 'Archivo entrega-tp2.tar.gz Terminado'
	@echo ' '
	@echo ' '

# Target de medida de performance
profile: all
	@echo ' '
	@echo ' '
	@echo '************************************';
	@echo 'Profiling hash compressor';
	@echo '************************************';
	@echo "$$(date)"
	-./ppmch -c 6 -f test.txt -e
	@echo "$$(date)"
	@echo ' '
	@echo ' '
	@echo '************************************';
	@echo 'Profiling hash decompressor';
	@echo '************************************';
	@echo "$$(date)"
	-./ppmch -d 6 -f test.txt.compressed -e
	@echo "$$(date)"
	@echo ' '
	@echo ' '
	@echo '************************************';
	@echo 'Profiling tree compressor';
	@echo '************************************';
	@echo "$$(date)"
	-./ppmct -c 6 -f test.txt -e
	@echo "$$(date)"
	@echo ' '
	@echo ' '
	@echo '************************************';
	@echo 'Profiling tree decompressor';
	@echo '************************************';
	@echo "$$(date)"
	-./ppmct -d 6 -f test.txt.compressed -e
	@echo "$$(date)"
	@echo ' '
	@echo ' '

# Target de preparación
prepare:
	@echo ' '
	@echo ' '
	@echo '*******************************************************'
	@echo 'Preparing directory structure'
	@echo '*******************************************************'
	-mkdir $(ALL_TARGET_DIRECTORIES)
	@echo ' '
	@echo ' '
	
# Target que buildea
build: prepare $(ALL_OBJS)
	@echo ' '
	@echo ' '
	@echo 'Done building all intermediate files'
	@echo ' '
	@echo ' '

# Función generadora de reglas para buildear cualquier *.o
define COMPILE_TEMPLATE 
$(1) : $(subst .o,.cpp, $(subst ./obj/,../source/,$(1)))
	@echo 'Building file: $$<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$$*.d" -MT"$$*.d" -o"$$@" "$$<"
	@echo 'Finished building: $$<'
	@echo ' '			
endef

# Generamos las reglas para buildear los .o
$(foreach obj,$(ALL_OBJS),$(eval $(call COMPILE_TEMPLATE,$(obj))))

# Inclusión de los archivos de manejo de dependencias
-include $(ALL_DEPENDENCIES)
