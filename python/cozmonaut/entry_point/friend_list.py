#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

from typing import Any, Dict, Text

from cozmonaut.entry_point import EntryPoint


class EntryPointFriendList(EntryPoint):
    """
    The entry point for friend listing.
    """

    def main(self, args: Dict[Text, Any]) -> int:
        """
        The main method.

        This entry point takes the following arguments:
         - friend_id: int - The target friend ID (optional)
         - sql_addr: str - The server address
         - sql_user: str - The login username
         - sql_pass: str - The login password
         - sql_data: str - The database name

        If a friend ID is provided, this code prints details about that friend
        to standard output. If no friend ID is provided, then we print details
        about all friends to standard output.

        :param args: A dictionary with above arguments
        :return: The exit code (zero for success, otherwise nonzero)
        """
        print('friend list')
        return 1  # TODO
