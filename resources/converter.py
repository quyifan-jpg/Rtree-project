import csv

input_file = "query.txt"
output_file = "query.csv"

rows = []
current_row = {}

# Read and process blocks
with open(input_file, "r") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        if "@@" in line:
            if len(current_row) > 0 :
                rows.append(current_row)
                current_row = {}
        elif ":" in line:
            key, value = line.split(":", 1)
            current_row[key.strip()] = value.strip()
# Append last row if not empty
if current_row:
    rows.append(current_row)

# Get all unique keys to use as headers
all_keys = sorted({k for row in rows for k in row.keys()})

# Write to CSV
with open(output_file, "w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=all_keys)
    writer.writeheader()
    writer.writerows(rows)

print(f"CSV file '{output_file}' created with {len(rows)} row(s) and {len(all_keys)} column(s).")
