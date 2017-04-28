#!/usr/bin/env bash

# This script is suitable to build the site after the correct directory
# structure is set up, particularly within the `web/sites` directory.

# Run this script with `sudo` privileges.

# Map directories and files.
script_root=$(dirname "$0")
cd "$script_root" || exit
project_root=$(pwd)
cd "$project_root" || exit
settings_file=$(find "$project_root" -type f | grep -E "\/settings\.php$")
site_root=$(dirname "$settings_file")

# Find all `.git` directories and pull new code for those repositories.
find "$project_root" -type d | grep -E "\.git$" | while read -r repo; do
  cd "$repo" || exit
  cd .. || exit
  echo "$PWD"
  git pull
done

# Check for `htaccess.patch` file.
if find "$site_root" -type f | grep -E "\/htaccess\.patch$"
  then
    # Check if `.htaccess` customizations are present.
    if ! grep -Fxq "# BEGIN CUSTOMIZATIONS" "$project_root"/web/.htaccess
      then
        # Apply patch to `.htaccess` file to add customizations.
        git apply -v "$site_root"/patches/htaccess.patch
    fi
fi

# Update the Drupal database.
cd "$site_root" || exit
drush updb -y
