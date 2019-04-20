#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

from abc import ABC, abstractmethod
from typing import Any, Dict, Text


class EntryPoint(ABC):
    """
    An entry point for some operation.
    """

    @abstractmethod
    def main(self, args: Dict[Text, Any]) -> int:
        """
        The main method.

        Each entry point's main method accepts a dictionary of arguments. These
        arguments come from the C code and are derived from the environment and
        command-line arguments.

        :param args: A dictionary with above arguments
        :return: The exit code (zero for success, otherwise nonzero)
        """
        raise NotImplementedError
