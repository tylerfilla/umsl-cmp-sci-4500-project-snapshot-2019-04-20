#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

import asyncio
from typing import Any, Dict, Text

import cozmo

from cozmonaut.entry_point import EntryPoint


class EntryPointInteract(EntryPoint):
    """
    The entry point for interactive mode.
    """

    def on_evt_new_raw_camera_image(self, evt: cozmo.robot.camera.EvtNewRawCameraImage, **kwargs):
        """
        Event handler for new camera frames.

        :param evt: The event
        :param kwargs: Remaining keyword arguments
        """

    async def robot_main(self, robot: cozmo.robot.Robot):
        """
        The main function for a robot.

        :param robot: The robot instance
        """

        # Enable color imaging on this Cozmo robot
        robot.camera.image_stream_enabled = True
        robot.camera.color_image_enabled = True

        # Start listening for new camera frames
        robot.camera.add_event_handler(cozmo.robot.camera.EvtNewRawCameraImage, self.on_evt_new_raw_camera_image)

        # Go for a drive
        # TODO: Actually do our work
        await robot.drive_off_charger_contacts().wait_for_completed()
        await robot.drive_straight(distance=cozmo.util.Distance(distance_mm=500),
                                   speed=cozmo.util.Speed(speed_mmps=50)).wait_for_completed()

    async def robot_battery_voltage_monitor_loop(self, robot: cozmo.robot.Robot):
        """
        The battery voltage monitor loop for a robot.

        :param robot: The robot voltage
        """

        while True:
            battery_voltage = robot.battery_voltage

            print(battery_voltage)

            # Target 0.5 Hz
            await asyncio.sleep(2)

    async def robot_imu_monitor_loop(self, robot: cozmo.robot.Robot):
        """
        The inertial motion unit (IMU) monitor loop for a robot.

        :param robot: The robot voltage
        """

        while True:
            accelerometer = robot.accelerometer.x_y_z
            gyro = robot.gyro.x_y_z

            # Target 10 Hz
            await asyncio.sleep(0.1)

    async def robot_wheel_speed_monitor_loop(self, robot: cozmo.robot.Robot):
        """
        The wheel speed monitor loop for a robot.

        :param robot: The robot instance
        """

        while True:
            speed_left = robot.left_wheel_speed.speed_mmps
            speed_right = robot.right_wheel_speed.speed_mmps

            # Target 10 Hz
            await asyncio.sleep(0.1)

    async def robot_monitor_loop(self, robot: cozmo.robot.Robot):
        """
        The main monitor loop for a robot.

        :param robot: The robot instance
        """

        # Delegate to subsystem monitor loops
        # This allows much finer control of timing for various subsystems
        # That way, e.g., wheel speed polling can be 10 Hz and battery polling can be < 1 Hz
        await asyncio.gather(
            asyncio.ensure_future(self.robot_battery_voltage_monitor_loop(robot)),
            asyncio.ensure_future(self.robot_imu_monitor_loop(robot)),
            asyncio.ensure_future(self.robot_wheel_speed_monitor_loop(robot)),
        )

    async def connection_main(self, conn: cozmo.conn.CozmoConnection):
        """
        The main function for SDK connections.

        :param conn: The SDK connection
        """
        # Get the robot for the connection
        robot = await conn.wait_for_robot()

        # Run main function and sensor loop for robot
        await asyncio.gather(
            asyncio.ensure_future(self.robot_main(robot)),
            asyncio.ensure_future(self.robot_monitor_loop(robot)),
        )

    def main(self, args: Dict[Text, Any]) -> int:
        """
        The main method.

        This entry point takes the following arguments:
         - sql_addr: str - The server address
         - sql_user: str - The login username
         - sql_pass: str - The login password
         - sql_data: str - The database name

        :param args: A dictionary with above arguments
        :return: The exit code (zero for success, otherwise nonzero)
        """

        # Get the event loop
        loop = asyncio.get_event_loop()

        # Make an SDK connection
        # This only supports one Cozmo at a time for now
        conn = cozmo.connect_on_loop(loop, connector=cozmo.AndroidConnector(adb_cmd='/home/tyler/.local/bin/adb'))

        # Start main function for connection
        task = asyncio.ensure_future(self.connection_main(conn))

        # Run connection main function
        loop.run_until_complete(task)
        return 0


# Stay put when starting up
cozmo.robot.Robot.drive_off_charger_on_connect = False
