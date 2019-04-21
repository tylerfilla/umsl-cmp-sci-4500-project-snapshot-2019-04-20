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
        print('got a camera frame')

    async def robot_main(self, robot: cozmo.robot.Robot):
        """
        The main function for individual robots.

        :param robot: The robot instance
        """

        # Enable color imaging on this Cozmo robot
        robot.camera.image_stream_enabled = True
        robot.camera.color_image_enabled = True

        # Start listening for new camera frames
        robot.camera.add_event_handler(cozmo.robot.camera.EvtNewRawCameraImage, self.on_evt_new_raw_camera_image)

    async def connection_main(self, conn: cozmo.conn.CozmoConnection):
        """
        The main function for SDK connections.

        :param conn: The SDK connection
        """
        # Get the robot for the connection
        robot = await conn.wait_for_robot()

        # Call main function for robot
        await self.robot_main(robot)

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
