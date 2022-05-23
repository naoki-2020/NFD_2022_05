#!/usr/bin/env bash
set -ex

if [[ $JOB_NAME == *"code-coverage" ]]; then
    # Generate an XML report (Cobertura format) and a detailed HTML report using gcovr
    # Note: trailing slashes are important in the paths below. Do not remove them!
    gcovr --object-directory=build \
          --output=build/coverage.xml \
          --exclude="$PWD/(examples|tests)" \
          --root=. \
          --xml

    # Generate also a detailed HTML output, but using lcov (better results)
    lcov --quiet \
         --capture \
         --directory . \
         --exclude "$PWD/ndn-cxx/detail/nonstd/*" \
         --exclude "$PWD/tests/*" \
         --no-external \
         --rc lcov_branch_coverage=1 \
         --output-file build/coverage.info

    genhtml --branch-coverage \
            --demangle-cpp \
            --legend \
            --output-directory build/lcov \
            --title "ndn-cxx unit tests" \
            build/coverage.info
fi
