import sys
import re
import struct
import glob
import os

# This is a very simple script to make a PDF file out of the output of a
# multipage symbol compression.
# Run ./jbig2 -s -p <other options> image1.jpeg image1.jpeg ...
# python pdf.py output > output.pdf

#Steven Lee, http://blog.rubypdf.com modify pdf.py to let it works under windows

class Ref:
  def __init__(self, x):
    self.x = x
  def __str__(self):
    return "%d 0 R" % self.x

class Dict:
  def __init__(self, values = {}):
    self.d = {}
    self.d.update(values)

  def __str__(self):
    s = ['<< ']
    for (x, y) in self.d.items():
      s.append('/%s ' % x)
      s.append(str(y))
      s.append("\n")
    s.append(">>\n")

    return ''.join(s)

global_next_id = 1

class Obj:
  next_id = 1
  def __init__(self, d = {}, stream = None):
    global global_next_id

    if stream is not None:
      d['Length'] = str(len(stream))
    self.d = Dict(d)
    self.stream = stream
    self.id = global_next_id
    global_next_id += 1

  def to_bytes(self):
    s = []
    s.append(str(self.d).encode('utf-8'))
    if self.stream is not None:
      s.append(b'stream\n')
      s.append(self.stream)
      s.append(b'\nendstream\n')
    s.append(b'endobj\n')

    return b''.join(s)

class Doc:
  def __init__(self):
    self.objs = []
    self.pages = []

  def add_object(self, o):
    self.objs.append(o)
    return o

  def add_page(self, o):
    self.pages.append(o)
    return self.add_object(o)

  def to_bytes(self):
    a = []
    j = [0]
    offsets = []

    def add(x):
      a.append(x)
      j[0] += len(x) + 1
    add(b'%PDF-1.4')
    for o in self.objs:
      offsets.append(j[0])
      add(('%d 0 obj' % o.id).encode("utf-8"))
      add(o.to_bytes())
    xrefstart = j[0]
    a.append(b'xref')
    a.append(('0 %d' % (len(offsets) + 1)).encode("utf-8"))
    a.append(b'0000000000 65535 f ')
    for o in offsets:
      a.append(('%010d 00000 n ' % o).encode("utf-8"))
    a.append(b'')
    a.append(b'trailer')
    a.append(('<< /Size %d\n/Root 1 0 R >>' % (len(offsets) + 1)).encode("utf-8"))
    a.append(b'startxref')
    a.append(str(xrefstart).encode("utf-8"))
    a.append(b'%%EOF')

    # sys.stderr.write(str(offsets) + "\n")

    return b'\n'.join(a)

def ref(x):
  return '%d 0 R' % x

def main(symboltable='symboltable', pagefiles=glob.glob('page-*')):
  doc = Doc()
  doc.add_object(Obj({'Type' : '/Catalog', 'Outlines' : ref(2), 'Pages' : ref(3)}))
  doc.add_object(Obj({'Type' : '/Outlines', 'Count': '0'}))
  pages = Obj({'Type' : '/Pages'})
  doc.add_object(pages)
  symd = doc.add_object(Obj({}, open(symboltable, 'rb').read()))
  page_objs = []

  for p in pagefiles:
    try:
      contents = open(p, 'rb').read()
    except IOError:
      sys.stderr.write("error reading page file %s\n"% p)
      continue
    (width, height) = struct.unpack('>II', contents[11:19])
    xobj = Obj({'Type': '/XObject', 'Subtype': '/Image', 'Width':
        str(width), 'Height': str(height), 'ColorSpace': '/DeviceGray',
        'BitsPerComponent': '1', 'Filter': '/JBIG2Decode', 'DecodeParms':
        ' << /JBIG2Globals %d 0 R >>' % symd.id}, contents)

    contents = Obj({}, ('q %d 0 0 %d 0 0 cm /Im1 Do Q' % (width, height)).encode("utf-8"))
    resources = Obj({'ProcSet': '[/PDF /ImageB]',
        'XObject': '<< /Im1 %d 0 R >>' % xobj.id})
    page = Obj({'Type': '/Page', 'Parent': '3 0 R',
        'MediaBox': '[ 0 0 %d %d ]' % (width, height),
        'Contents': ref(contents.id),
        'Resources': ref(resources.id)})
    [doc.add_object(x) for x in [xobj, contents, resources, page]]
    page_objs.append(page)

    pages.d.d['Count'] = str(len(page_objs))
    pages.d.d['Kids'] = '[' + ' '.join([ref(x.id) for x in page_objs]) + ']'
#How to Let Python Send Binary data to stdout under Windows
#http://blog.rubypdf.com/2009/11/03/how-to-let-python-send-binary-data-to-stdout-under-windows/
#added by Steven Lee
  if sys.platform == "win32":
  	import os, msvcrt
  	msvcrt.setmode(sys.stdout.fileno(), os.O_BINARY)
  #print(str(doc))
  open('output.pdf', 'wb').write(doc.to_bytes())


def usage(script, msg):
  if msg:
    sys.stderr.write("%s: %s\n"% (script, msg))
  sys.stderr.write("Usage: %s [file_basename] > out.pdf\n"% script)
  sys.exit(1)


if __name__ == '__main__':

  if len(sys.argv) == 2:
    sym = sys.argv[1] + '.sym'
    pages = glob.glob(sys.argv[1] + '.[0-9]*')
  elif len(sys.argv) == 1:
    sym = 'symboltable'
    pages = glob.glob('page-*')
  else:
    usage(sys.argv[0])

  if not os.path.exists(sym):
    usage("symbol table %s not found!"% sym)
  elif len(pages) == 0:
    usage("no pages found!")

  main(sym, pages)