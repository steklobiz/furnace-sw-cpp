Problem:	
How should Sample (temperature) be distributed to consumers?

Chosen direction:	
Pull — Consumers call get_last_sample() when they need data

Rationale:
Temperature changes slowly. Consumers control their own sampling rate. Producer does not need to know consumers

Details:
Consumers call get_last_sample() — TC_Parser has no consumer list.
