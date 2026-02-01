from krita import Krita, InfoObject
import os

app = Krita.instance()
doc = app.activeDocument()

if not doc:
    raise RuntimeError("No active document open")

if not doc.fileName():
    raise RuntimeError("Document must be saved before exporting")
    
# ========== CONFIG ==========

# Relative path -> export
doc_dir = os.path.dirname(doc.fileName())
export_dir = os.path.join(doc_dir, "export")
os.makedirs(export_dir, exist_ok=True)

# PNG export settings
cfg = InfoObject()
cfg.setProperty("alpha", True)           # include transparency
cfg.setProperty("compression", 9)         # best compression
cfg.setProperty("forceSRGB", True)
cfg.setProperty("indexed", False)
cfg.setProperty("interlaced", False)
cfg.setProperty("saveSRGBProfile", False)

# ============================

root = doc.rootNode()
groups = [n for n in root.childNodes() if n.type() == "grouplayer"]

if len(groups) < 2:
    raise RuntimeError("Document must have at least two top-level group layers")

groupA = groups[0]
groupB = groups[1]

# Get all paint layers from each group
layersA = [l for l in groupA.childNodes() if l.type() == "paintlayer"]
layersB = [l for l in groupB.childNodes() if l.type() == "paintlayer"]

def hide_all():
    # Hide all layers in both groups
    for g in [groupA, groupB]:
        for layer in g.childNodes():
            layer.setVisible(False)

export_count = 0

# ---- EXPORT COMBINATIONS ----
for la in layersA:
    for lb in layersB:
        hide_all()
        la.setVisible(True)
        lb.setVisible(True)
        doc.refreshProjection()

        filename = f"{la.name()}{lb.name()}.png"
        if filename[0] != '-':
            filepath = os.path.join(export_dir, filename)
            app.setBatchmode(True)
            doc.exportImage(filepath, cfg)
            export_count += 1

# ---- EXPORT EACH 'BaseImage' LAYER ALONE ----
for la in layersA:
    hide_all()
    la.setVisible(True)
    doc.refreshProjection()

    filename = f"{la.name()}.png"
    filepath = os.path.join(export_dir, filename)
    app.setBatchmode(True)
    doc.exportImage(filepath, cfg)
    export_count += 1

app.setBatchmode(False)
print(f"Exported {export_count} images.")
