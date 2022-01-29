#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

import asyncio
from typing import Any, Dict, Text

import PIL.Image
import base
import cv2

from cozmonaut.entry_point import EntryPoint


class EntryPointInteract(EntryPoint):
    """
    The entry point for interactive mode.
    """

    def __init__(self):
        # Add a robot
        base.add_robot(0)

        # Get monitor and tracker
        self.monitor = base.get_monitor(0)
        self.tracker = base.get_tracker(0)

        # Open first video capture device
        self.capture = cv2.VideoCapture(0)

    async def capture_loop(self):
        while True:
            # Get the next frame
            ret, frame = self.capture.read()

            # Show the frame
            cv2.imshow('Output', frame)

            # Send the camera frame off for face tracking
            self.tracker.push_camera(PIL.Image.fromarray(frame))

            # Poll window and stop on Q key down
            if cv2.waitKey(1) == ord('q'):
                break

            # Yield control
            await asyncio.sleep(0)

    async def face_loop(self):
        while True:
            # Wait for a new track
            track = await self.tracker.wait_for_new_track()

            print(f'a new track was found: {track}')

            # Yield control
            await asyncio.sleep(0)

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

        # Get event loop
        loop = asyncio.get_event_loop()

        # Run coroutines
        loop.run_until_complete(asyncio.gather(
            self.capture_loop(),
            self.face_loop(),
        ))
        return 0
