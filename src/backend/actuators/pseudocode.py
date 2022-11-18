
# Modes
DISTANCE_CONTROL = 0
VELOCITY_CONTROL = 1

# Variables
delta_distance = 0 # Changes in distance between the last 2 readings from the simulator
delta_time = 0 # Changes in time between the last 2 readings from the simulator
actual_distance, desired_distance, distance_padding, mode = 0, 0, 0, 0
actual_velocity, desired_velocity, velocity_padding, velocity = 0, 0, 0, 0
minimum_speed = 0
minimum_distance = 0
delta_time = 0

def ACC():
    while True:
        relative_velocity = -delta_distance / delta_time    # The relative velocity between our car and the car in front of us
                                                            # If distance is decreasing, we are moving faster, rel_vel > 0
                                                            # Otherwise, we are moving slower, rel_vel < 0
        lead_velocity = actual_velocity - relative_velocity # Lead vehicle velocity
                                                            # v = 53, v_other = 50 --> relative = 3 --> 
                                                            # v = 50, v_other = 53 --> relative = -3 -->

        if (actual_distance < desired_distance):
            mode = DISTANCE_CONTROL
        elif ((actual_velocity > desired_velocity or actual_distance >= desired_distance)):
            mode = VELOCITY_CONTROL

        if (mode == DISTANCE_CONTROL):
            # Find the speed of the lead vehicle
            # Match the lead vehicle's speed
            if (actual_velocity > lead_velocity): # We are approaching the lead vehicle
                time_to_lead = (minimum_distance - actual_distance) / relative_velocity
                desired_acceleration = (lead_velocity - actual_velocity) / time_to_lead
                assign_throttle_and_brake(actual_velocity, lead_velocity, desired_acceleration)
            else: # The lead vehicle is moving away from us, probably never going to happen
                pass # Do nothing
        elif (mode == VELOCITY_CONTROL):
            # Match the desired velocity
            if (abs(actual_velocity - desired_velocity) >= velocity_padding):
                assign_throttle_and_brake(actual_velocity, desired_velocity)
                

def assign_throttle_and_brake(actual_speed, desired_speed, desired_acceleration = None):
    pass


COAST_THRESHOLD= 0.5 * actual_velocity   
# throttle > COAST_THRESHOLD # -> accelerate (ex: 21 --> 1 m/s^2)
# throttle = COAST_THRESHOLD # -> coast (assume fixed friction, so this is the minimum throttle to keep the same speed)
# throttle < COAST_THRESHOLD # -> decelerate at rate (ex: 19 --> -1 m/s^2)
# brake > 0                  # -> decelerate at rate (ex: 1 --> -21 m/s^2)
