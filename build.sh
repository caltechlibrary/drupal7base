#!/usr/bin/env bash

# This script is suitable to build the site after the correct directory
# structure is set up, particularly within the `web/sites` directory.

# Run it with `sudo` privileges.

# Get the directory of this script.
project_root=`dirname $0`

# Change directory to web root.
cd ${project_root}/web

# Update Drupal codebase via Git.
git pull

# get list of custom module directories
# loop through list and git pull inside each one

# get list of custom theme directories
# loop through list and git pull inside each one

# run drush updb
