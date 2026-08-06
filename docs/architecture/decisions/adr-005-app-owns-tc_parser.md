# Problem:
Who owns the TC_Parser and how do consumers access Samples?

# Chosen direction:
Root App owns TC_Parser — Consumers receive a pointer via init() and pull 
Samples via get_last_sample()

# Rationale:
TC_Parser does not need to know its consumers. Pull mode is simpler, avoids callbacks, and matches the periodic nature of temperature data

s