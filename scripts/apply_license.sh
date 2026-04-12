#!/bin/bash
# Applying GNU GPL v3.0 header to all source files
# Copyright (C) 2026 Robson Rojas

HEADER="/*
 * Copyright (C) 2026 Robson Rojas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */"

JS_HEADER="/**
 * Copyright (C) 2026 Robson Rojas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */"

# Directories to process
DIRS=("src/esp32-s3-cpp/main" "src/telemetry" "src/components" "src/pages" "supabase/functions")

for dir in "${DIRS[@]}"; do
    if [ -d "$dir" ]; then
        echo "Processing $dir..."
        find "$dir" -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.ts" -o -name "*.tsx" -o -name "*.js" \) | while read file; do
            # Skip external SDKs
            if [[ "$file" == *"edge-impulse-sdk"* ]]; then
                continue
            fi
            
            # Check if license already applied
            if grep -q "GNU General Public License" "$file"; then
                echo "  Skipping $file (already licensed)"
                continue
            fi

            echo "  Applying license to $file"
            if [[ "$file" == *.ts || "$file" == *.tsx || "$file" == *.js ]]; then
                echo -e "$JS_HEADER\n\n$(cat "$file")" > "$file"
            else
                echo -e "$HEADER\n\n$(cat "$file")" > "$file"
            fi
        done
    fi
done

echo "License application complete."
