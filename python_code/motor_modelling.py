import numpy as np
from matplotlib import pyplot as plt

applied_force_radius = 08.25 #centimeters

pwm_to_volts_stalled = np.zeros(shape=(13,2))
pwm_to_current_stalled = np.zeros(shape=(13,2))
pwm_to_current_inertia = np.zeros(shape=(13,2))
pwm_to_ang_vel = np.zeros(shape=(13,2))
pwm_to_torque = np.zeros(shape=(13,2))

print(pwm_to_volts_stalled)

# NB: All calculations were done with motor rotating CCW (digitalWrite(DIR_B, LOW))
pwms = [255, 240, 230, 220, 200, 180, 160, 140, 120, 100, 80, 60, 40]
vrms_stalled = [9.72, 9.53, 9.35, 9.31, 9.02, 8.72, 8.38,7.82, 6.95,6.09,5.52, 4.85, 4.04]
current_stalled =       [0.724, 0.621, 0.575, 0.515, 0.388, 0.262, 0.217, 0.174, 0.137, 0.101, 0.068, 0.048, 0.021]
current_inertia_wheel = [0.027, 0.025, 0.025, 0.025, 0.025, 0.024, 0.023, 0.023, 0.023, 0.022, 0.021, 0.019, 0.015]
ang_vel = [52.48, 50.2, 50.05, 49.3, 48.58, 47.48, 45.78, 44.00, 40.55, 36.56, 30.85, 21.35, 6.55]
forces = [92.4, 95, 80, 70,57.7, 52, 46, 39, 31.4, 25.5,19.5, 12.5, 7] #grams

for i, val in enumerate(pwms):
    pwm_to_volts_stalled[i][0] = pwms[i]
    pwm_to_volts_stalled[i][1] = vrms_stalled[i]

    pwm_to_current_stalled[i][0] = pwms[i]
    pwm_to_current_stalled[i][1] = current_stalled[i]

    pwm_to_current_inertia[i][0] = pwms[i]
    pwm_to_current_inertia[i][1] = current_inertia_wheel[i]

    pwm_to_ang_vel[i][0] = pwms[i]
    pwm_to_ang_vel[i][1] = ang_vel[i]

    pwm_to_torque[i][0] = pwms[i]
    pwm_to_torque[i][1] = forces[i]*applied_force_radius/1000

print(pwm_to_volts_stalled)

fig, axes = plt.subplots(5)
axes[0].plot(pwm_to_volts_stalled[:,0], pwm_to_volts_stalled[:,1], color='blue', linewidth=1)
axes[1].plot(pwm_to_current_stalled[:,0], pwm_to_current_stalled[:,1], color='red', linewidth=1)
axes[2].plot(pwm_to_current_inertia[:,0], pwm_to_current_inertia[:,1], color='green', linewidth=1)
axes[3].plot(pwm_to_ang_vel[:,0], pwm_to_ang_vel[:,1], color='orange', linewidth=1)
axes[4].plot(pwm_to_torque[:,0], pwm_to_torque[:,1], color='cyan', linewidth=1)
for a in axes:
    a.set(xlabel='pwm')
axes[0].set(ylabel='V_rms')
axes[1].set(ylabel='stalled current (A_rms)')
axes[2].set(ylabel='rotating current (A_rms)')
axes[3].set(ylabel='ang_vel (rad/sec)')
axes[4].set(ylabel='torques (kg*cm)')
plt.show()