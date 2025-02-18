import numpy as np
from bfio import BioReader
from pathlib import Path

path = str(Path(__file__).parent.resolve())

with BioReader(path + '/data/int/p0_y1_r1_c0.ome.tif') as br:
    int_p0_y1_r1_c0 = br.read()
    
with BioReader(path + '/data/int/p0_y1_r1_c1.ome.tif') as br:
    int_p0_y1_r1_c1 = br.read()
    
with BioReader(path + '/data/seg/p0_y1_r1_c0.ome.tif') as br:
    seg_p0_y1_r1_c0 = br.read()
    
with BioReader(path + '/data/seg/p0_y1_r1_c1.ome.tif') as br:
    seg_p0_y1_r1_c1 = br.read()

tissuenet_int = np.array([int_p0_y1_r1_c0, int_p0_y1_r1_c1])
tissuenet_seg = np.array([seg_p0_y1_r1_c0, seg_p0_y1_r1_c1])
