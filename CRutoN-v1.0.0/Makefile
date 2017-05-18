#-------------------------------------------------------------------------------
#	Makefile
#
#	All cpp files in immediate subdirectories of SOURCE_ROOT_DIR will be 
#	compiled and linked to create EXECUTABLE.
#
#	Author				: Paul Gainer
#	Created				: 15/09/14
#   Last Modified		: 16/09/14
#-------------------------------------------------------------------------------
CC       		 	:= g++
C++STANDARD			:= c++11
EXECUTABLE			:= cruton

#-------------------------------------------------------------------------------
#	set target operating system, this must be set to LINUX or WINDOWS
#-------------------------------------------------------------------------------
TARGET_OS			:= WINDOWS

#-------------------------------------------------------------------------------
#	set optimization switches
#-------------------------------------------------------------------------------
OFLAGS				:= -O3

#-------------------------------------------------------------------------------
#	set compiler switches
#-------------------------------------------------------------------------------
CFLAGS				:= -std=$(C++STANDARD) $(OFLAGS)

#-------------------------------------------------------------------------------
#	set source and target directories
#-------------------------------------------------------------------------------
SOURCE_ROOT_DIR		:= src
BUILD_ROOT_DIR		:= build
BIN_DIR				:= bin
INCLUDES			:= -I./include

#-------------------------------------------------------------------------------
#	analyse source files and directory structure
#-------------------------------------------------------------------------------
SOURCE_DIR_SLASH	:= $(sort $(dir $(wildcard $(SOURCE_ROOT_DIR)/*/)))
SOURCE_DIR 			:= $(patsubst %/,%,$(SOURCE_DIR_SLASH))
SRC					:= $(foreach sdir,$(SOURCE_DIR),$(wildcard $(sdir)/*.cpp))
OBJ					:= $(patsubst $(SOURCE_ROOT_DIR)/%.cpp,$(BUILD_ROOT_DIR)/%.o,$(SRC))
BUILD_DIR_LINUX		:= $(subst $(SOURCE_ROOT_DIR),$(BUILD_ROOT_DIR),$(SOURCE_DIR))
BUILD_DIR_WINDOWS	:= $(subst /,\,$(BUILD_DIR_LINUX))

ifeq ($(TARGET_OS), LINUX)
	#---------------------------------------------------------------------------
	#	define variables for LINUX
	#---------------------------------------------------------------------------
	BUILD_DIR = $(BUILD_DIR_LINUX)
	EXECUTABLE_NAME = $(EXECUTABLE)
	REMOVE_BUILD_FILES = rm -rf $(BUILD_DIR)
	REMOVE_EXECUTABLE = rm $(BIN_DIR)/$(EXECUTABLE_NAME)
	BUILD_DIRECTORIES = @mkdir -p $@
else
	ifeq ($(TARGET_OS), WINDOWS)
		#-----------------------------------------------------------------------
		#	define variables for WINDOWS
		#-----------------------------------------------------------------------
		BUILD_DIR = $(BUILD_DIR_WINDOWS)
		EXECUTABLE_NAME = $(EXECUTABLE).exe
		REMOVE_BUILD_FILES = rmdir /Q /S $(BUILD_DIR)
		REMOVE_EXECUTABLE = del $(BIN_DIR) /Q
		BUILD_DIRECTORIES = @mkdir $@
	else
		#-----------------------------------------------------------------------
		#	unknown target OS, exit with an error
		#-----------------------------------------------------------------------
		$(error invalid value for TARGET_OS, please use LINUX or WINDOWS)
	endif
endif

#-------------------------------------------------------------------------------
#	build object goals
#-------------------------------------------------------------------------------
vpath %.cpp $(SOURCE_DIR)

define build-object
$1/%.o: %.cpp
	@echo creating object file $$@...
	@$(CC) $(CFLAGS) $(INCLUDES) -c $$< -o $$@
endef

$(foreach sub_dir,$(BUILD_DIR_LINUX),$(eval $(call build-object,$(sub_dir))))

#-------------------------------------------------------------------------------
#	build directory structure if needed, then the executable
#-------------------------------------------------------------------------------
all: check_build_sub_dirs $(BIN_DIR)/$(EXECUTABLE_NAME)

$(BIN_DIR)/$(EXECUTABLE_NAME): $(OBJ)
	@echo linking object files...
	@$(CC) $(CFLAGS) $^ -o $@
	@echo $(EXECUTABLE_NAME) compiled successfully
	
check_build_sub_dirs: $(BUILD_DIR)

$(BUILD_DIR):
	$(BUILD_DIRECTORIES)

#-------------------------------------------------------------------------------
#	clean up object files, build directories, and executable
#-------------------------------------------------------------------------------
clean:
	@$(REMOVE_BUILD_FILES)
	@$(REMOVE_EXECUTABLE)
