# GenerateVersion.cmake
find_package(Git REQUIRED)

# 1. Get the abbreviated Git Commit Hash
execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY "${SRC_DIR}"
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 2. Check if the repository is dirty
execute_process(
    COMMAND ${GIT_EXECUTABLE} status --porcelain
    WORKING_DIRECTORY "${SRC_DIR}"
    OUTPUT_VARIABLE GIT_STATUS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 3. Handle Conditional Date and Formatting
if(NOT "${GIT_STATUS}" STREQUAL "")
    # DIRTY REPO: Get compilation/current time
    string(TIMESTAMP CURRENT_DATE_TIME "%Y-%m-%d %H:%M")
    set(REPO_VERSION_STRING "${CURRENT_DATE_TIME} (${GIT_HASH}-dirty)")
else()
    # CLEAN REPO: Get commit date and time
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --format=%cd --date=format:%Y-%m-%d\ %H:%M HEAD
        WORKING_DIRECTORY "${SRC_DIR}"
        OUTPUT_VARIABLE COMMIT_DATE_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(REPO_VERSION_STRING "${COMMIT_DATE_TIME} (${GIT_HASH})")
endif()

# 4. Generate the version_GENERATED_.h file only if it changed (prevents constant clean builds)
set(VERSION_GENERATED_H_CONTENT "
#ifndef VERSION_GENERATED_H
#define VERSION_GENERATED_H
#define APP_VERSION \"${REPO_VERSION_STRING}\"
#endif // VERSION_GENERATED_H
")

if(EXISTS "${BIN_DIR}/version_GENERATED_.h")
    file(READ "${BIN_DIR}/version_GENERATED_.h" EXISTING_CONTENT)
endif()

if(NOT "${VERSION_GENERATED_H_CONTENT}" STREQUAL "${EXISTING_CONTENT}")
    file(WRITE "${BIN_DIR}/version_GENERATED_.h" "${VERSION_GENERATED_H_CONTENT}")
endif()