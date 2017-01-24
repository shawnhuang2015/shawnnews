
import sys;
import random;

case_ids = int(sys.argv[1]);
fract_fraud = float(sys.argv[2]);
in_file = sys.argv[3];



def print_to_screen(tokens) :
  line = ",".join(map(str, tokens));
  print line; 

handle = open (in_file, "r");

for line in handle:
  line = line.rstrip('\n');
  tokens = line.split(",");
  if random.random() < fract_fraud:
    tokens[5] = random.randint(0, case_ids);

  print_to_screen(tokens);


handle.close();


