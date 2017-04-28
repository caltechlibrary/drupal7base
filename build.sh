#!/usr/bin/env bash

# This script is suitable to build the site after the correct directory
# structure is set up, particularly within the `web/sites` directory.

# Run this script with `sudo` privileges.

# Map directories and files.
script_root=$(dirname "$0")
cd "$script_root" || exit
project_root=$(pwd)
web_root="${project_root}/web"
settings_file=$(find "$project_root" -type f | grep -E "\/settings\.php$")
site_specific_dir=$(dirname "$settings_file")

# Find all `.git` directories and pull new code for those repositories.
find "$project_root" -type d | grep -E "\.git$" | while read -r repo; do
  cd "$repo" || exit
  cd .. || exit
  echo "$PWD"
  git pull
done

# Check for `htaccess.patch` file.
if find "$site_specific_dir" -type f | grep -E "\/htaccess\.patch$"
  then
    # Revert `.htaccess` in order to apply patch.
    git checkout "$web_root"/.htaccess
    # Apply patch to `.htaccess` file to add customizations.
    git apply -v "$site_specific_dir"/patches/htaccess.patch
fi

# Update the Drupal database.
cd "$site_specific_dir" || exit
drush updb -y
