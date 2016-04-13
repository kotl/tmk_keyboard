import re

# [{y:-0.875,x:3.5},"1\nF1\n\n\n\n\nF1\n5/4",{x:1},"3\nF3\n\n\n\n\nF3\n5/2",{x:9},"8\nF8\n\n\n\n\nF8\n0/7",{x:1},"0\nF10\n\n\n\n\nF10\n0/1"],
# [{y:-0.875,x:6.5},"4\nF4\n\n\n\n\nF4\n5/1",{x:7},"7\nF7\n\n\n\n\nF7\n0/6"],
# [{y:-0.875,x:2.5},"GRV\n\n\n\n\n\n\n5/5",{x:15},"MINS\n\n\n\n\n\n\n0/2"],

def convert(layer, val, fns):
  val = val.replace("TGL(", "ACTION_LAYER_TOGGLE(")
  val = val.replace("MOM(", "ACTION_LAYER_MOMENTARY(")
  val = val.upper().split(',')
  if len(val) == 1:
    if ')' in val[0]:
      return val[0]
    if val[0] == '':
      if layer == 0:
        return 'KC_NO'
      else:
        return 'KC_TRNS'
    return ('KC_' + val[0])
  else:
    fn = int(val[0][2:])
    if len(val) > 2:
      ret = 'ACTION_MODS_KEY('
      for idx,mod in enumerate(val):
        if idx < len(val)-1 and idx > 1 :
          ret = ret + '|' 
        if idx >0:
          if idx < len(val)-1:
            ret = ret + 'MOD_' + mod
          else:
            ret = ret + ', KC_' + mod + ')'
    else:
      ret = val[1]
    fns[fn] = "  " + ret + ",      // " + val[0]
    return ('KC_' + val[0])

def print_layer(layer, matrix):
  print '[' + str(layer) + '] = { \\'

  for ridx, row in enumerate(matrix):
    prn = '{ '
    for cidx, cell in enumerate(row):
      if cidx != 0:
	prn = prn + ','
      prn = prn + cell[layer]
    prn = prn + '}'
    if ridx != len(matrix)-1:
     prn = prn + ','
    print prn + '  \\'
  print '},  '


def print_fnmap(matrix, fns):
  print 'static const uint16_t PROGMEM fn_actions[] = {'
  for fn in sorted(fns):
    print fns[fn]
  print '};'

def init_matrix():
  matrix = []
  for ridx in range(0,10):
    row = []
    for cidx in range(0,8):
      cell = ['KC_NO', 'KC_NO', 'KC_NO']
      row.append(cell)
    matrix.append(row)
  return matrix

def main():
  matrix = init_matrix()
  p1 = re.compile("(.*?\"(.*?)\")")
  p2 = re.compile("(.*?)\\\\n(.*?)\\\\n\\\\n\\\\n\\\\n\\\\n(.*?)\\\\n(.+)")
  p3 = re.compile("(\d+)/(\d+)")
  fns = {}
  with open('layout.txt') as layout:
    for line in layout: 
      matches = p1.findall(line)
      for match in matches:
        matches2 = p2.findall(match[1])
        for match2 in matches2:
	  # print match2
 	  (layer0, layer1, layer2, coord) = match2
 	  layers = [convert(0, layer0, fns),
                    convert(1, layer1, fns),
                    convert(2, layer2, fns)]
          (x,y) = p3.findall(coord)[0]
	  matrix[int(x)][int(y)] = layers
  # Printing keymap
  print 'static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {'
  print_layer(0, matrix)
  print_layer(1, matrix)
  print_layer(2, matrix)
  print '};'
  # Printing FN map
  print_fnmap(matrix, fns)

if __name__ == "__main__":
  main()
