#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

from typing import Any, Dict, Text

from cozmonaut.entry_point import EntryPoint


class EntryPointFriendRemove(EntryPoint):
    """
    The entry point for manual friend removal.
    """

    def main(self, args: Dict[Text, Any]) -> int:
        """
        The main method.

        This entry point takes the following arguments:
         - friend_id: int - The target friend ID
         - sql_addr: str - The server address
         - sql_user: str - The login username
         - sql_pass: str - The login password
         - sql_data: str - The database name

        :param args: A dictionary with above arguments
        :return: The exit code (zero for success, otherwise nonzero)
        """
        print('friend remove')
        return 1  # TODO
