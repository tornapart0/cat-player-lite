"""Rebuild the OpenSCAD enclosure as editable STEP solids (mm).
Requires cadquery 2.8 and shapely 2.2. Run from any directory.
Outline uses the same 64 vertices as OpenSCAD; circular bores are analytic.
"""
from pathlib import Path
import math,json
import cadquery as cq
from shapely.geometry import Polygon
from shapely.ops import unary_union
ROOT=Path(__file__).resolve().parent
outline=unary_union([Polygon([(58*math.cos(i*math.tau/64),42*math.sin(i*math.tau/64)) for i in range(64)]),Polygon([(-50,23),(-42,64),(-19,34),(-15,22)]),Polygon([(50,23),(42,64),(19,34),(15,22)])])
def prism(poly,z,h):
    outer=cq.Wire.makePolygon([cq.Vector(x,y,z) for x,y in poly.exterior.coords],close=True)
    holes=[cq.Wire.makePolygon([cq.Vector(x,y,z) for x,y in ring.coords],close=True) for ring in poly.interiors]
    return cq.Solid.extrudeLinear(outer,holes,cq.Vector(0,0,h))
def cyl(x,y,z,r,h):return cq.Solid.makeCylinder(r,h,cq.Vector(x,y,z))
pts=[(-39,22),(39,22),(-36,-26),(36,-26)]
def shell(depth,boss_d,boss_h):
    s=prism(outline,0,depth).cut(prism(outline.buffer(-2.2,join_style=2),2.2,depth))
    for x,y in pts:s=s.fuse(cyl(x,y,2.2,boss_d/2,boss_h))
    return s
front=shell(8,7.2,5.8)
# OpenSCAD rounded square [20,10] offset radius 1.5; located at y=8.
screen=Polygon([(-10,3),(10,3),(10,13),(-10,13)]).buffer(1.5,quad_segs=16)
front=front.cut(prism(screen,-1,10))
for x,y,r in [(-35,3,5.25),(35,3,5.25),(37,-20,3.7)]+[(x,y,1.2) for x,y in pts]+[(x,y,1.25) for x in range(-10,11,5) for y in [-34,-29]]:
    front=front.cut(cyl(x,y,-1,r,10))
back=shell(16,8,12.8)
a=outline.buffer(-2.55,join_style=2); b=outline.buffer(-3.85,join_style=2)
back=back.fuse(prism(a.difference(b),14.1,1.7))
# Support the inset ring: the original SCAD ring floated 0.35 mm from the wall.
back=back.fuse(prism(outline.buffer(-2.2,join_style=2).difference(b),14.1,0.5))
for cx,cy,cz,sx,sy,sz in [(-58,-5,8.8,12,11,5),(0,-42,7.68,19,12,4)]:
    back=back.cut(cq.Solid.makeBox(sx,sy,sz,cq.Vector(cx-sx/2,cy-sy/2,cz-sz/2)))
for x,y in pts:back=back.cut(cyl(x,y,6,0.85,12))
assembly=cq.Compound.makeCompound([front,back.mirror('XY').translate((0,0,29))])
report={}
for name,s in [('front',front),('back',back),('assembly',assembly)]:
    assert s.isValid(),name
    assert len(s.Solids())==(2 if name=='assembly' else 1),(name,len(s.Solids()))
    path=ROOT/f'cat_player_lite_{name}.step';cq.exporters.export(s,str(path))
    imported=cq.importers.importStep(str(path)).val()
    assert imported.isValid()
    assert abs(imported.Volume()-s.Volume())<0.01
    bb=s.BoundingBox();report[name]={'valid':True,'solids':len(s.Solids()),'volume_mm3':round(s.Volume(),3),'bounds_mm':[round(bb.xlen,3),round(bb.ylen,3),round(bb.zlen,3)],'round_trip_volume_error_mm3':abs(imported.Volume()-s.Volume())}
# Mesh render uses the exported shapes, not an illustration of a hypothetical case.
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
fig=plt.figure(figsize=(12,7));fig.patch.set_facecolor('#f4f1ed')
ax=fig.add_subplot(111,projection='3d');ax.set_facecolor('#f4f1ed')
for s,color in [(front,'#e8a0ac'),(back.translate((135,0,0)),'#64869b')]:
    v,t=s.tessellate(0.18);ax.add_collection3d(Poly3DCollection([[(v[i].x,v[i].y,v[i].z) for i in tri] for tri in t],facecolors=color,linewidths=0,alpha=1,shade=True,lightsource=matplotlib.colors.LightSource(azdeg=220,altdeg=45)))
ax.set(xlim=(-65,200),ylim=(-50,70),zlim=(0,40));ax.set_box_aspect((265,120,70));ax.view_init(elev=52,azim=-65);ax.set_axis_off();fig.suptitle('Cat Player Lite — front and rear enclosure shells',fontsize=18);fig.text(.5,.06,'STEP geometry preview · dimensions in mm · module mounting and physical fit remain to be verified',ha='center',fontsize=10)
fig.savefig(ROOT/'cat_player_lite_step_preview.png',dpi=150,bbox_inches='tight');plt.close(fig)
(ROOT/'step-validation.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
