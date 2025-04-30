from PIL import Image
import os

# Paths
base_dir = 'base'
decal_dir = 'decals'
output_dir = 'output'

# Ensure output directory exists
os.makedirs(output_dir, exist_ok=True)

TARGET_SIZE = (16, 16)

for base_file in os.listdir(base_dir):
    if not base_file.lower().endswith('.png'):
        continue

    base_path = os.path.join(base_dir, base_file)
    base_img = Image.open(base_path).convert("RGBA")

    # Ensure base is 16x16, but don't reposition — just center on transparent canvas if needed
    if base_img.size != TARGET_SIZE:
        padded_base = Image.new("RGBA", TARGET_SIZE, (0, 0, 0, 0))
        paste_x = (TARGET_SIZE[0] - base_img.width) // 2
        paste_y = (TARGET_SIZE[1] - base_img.height) // 2
        padded_base.paste(base_img, (paste_x, paste_y), base_img)
    else:
        padded_base = base_img

    base_name = os.path.splitext(base_file)[0]

    # Export base-only version, unmodified layout
    base_output_path = os.path.join(output_dir, f'{base_name}.png')
    padded_base.save(base_output_path)

    for decal_file in os.listdir(decal_dir):
        if not decal_file.lower().endswith('.png'):
            continue

        decal_path = os.path.join(decal_dir, decal_file)
        decal_img = Image.open(decal_path).convert("RGBA")

        # Paste decal in bottom-right corner
        x = TARGET_SIZE[0] - decal_img.width
        y = TARGET_SIZE[1] - decal_img.height

        combined = padded_base.copy()
        combined.paste(decal_img, (x, y), decal_img)

        # Output filename (no underscore)
        decal_name = os.path.splitext(decal_file)[0]
        output_filename = f'{base_name}{decal_name}.png'
        output_path = os.path.join(output_dir, output_filename)
        combined.save(output_path)

print("✅ All base-only and base+decal combinations exported.")
