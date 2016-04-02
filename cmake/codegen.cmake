macro(sc_codegen_ex Target SrcPath OutputPath)
	# fetch all include directories for the project target
	get_property(DIRECTORIES TARGET ${Target} PROPERTY INCLUDE_DIRECTORIES)

	# build the include directory flags
	foreach (DIRECTORY ${DIRECTORIES})
	    set(META_FLAGS ${META_FLAGS} "\\-I${DIRECTORY}")
	endforeach ()

	get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
	foreach(dir ${dirs})
  		set(META_FLAGS ${META_FLAGS} "\\-I${dir}")
	endforeach()

	# include the system directories
	if (MSVC)
	    # assume ${VS_VERSION} is the version of Visual Studio being used to compile
	    set(META_FLAGS ${META_FLAGS} 
	        "\\-IC:/Program Files (x86)/Microsoft Visual Studio ${VS_VERSION}/VC/include"
	    )
	else ()
	    # you can figure it out for other compilers :)
	    message(FATAL_ERROR "System include directories not implemented for this compiler.")
	endif ()

	include_directories(${OutputPath})
	add_custom_command(
    	PRE_BUILD
    	TARGET ${Target}
		DEPENDS ${Headers}
		COMMAND call "${SC_PARSE_TOOL}"
		--target 	"${Target}"
		--source 	"${SrcPath}"
		--output 	"${OutputPath}"
		--flags 	${META_FLAGS}
	)
endmacro(sc_codegen_ex)

macro(sc_codegen Target SrcPath)
	sc_codegen_ex(${Target} ${SrcPath} "${CMAKE_CURRENT_BINARY_DIR}/${Target}_gen")
endmacro(sc_codegen)