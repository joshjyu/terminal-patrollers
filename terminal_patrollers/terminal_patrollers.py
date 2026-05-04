import curses


def _run_confirm_exit(stdScreen: "curses.window") -> bool:
    """
    Prompts the user to confirm exiting via an interactive menu.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
    Returns:
      bool: True if the user confirms exit, False otherwise.
    """
    options = ["YES, QUIT TO THE TERMINAL", "NO, RETURN TO THE MENU"]
    currentSelection = 1  # Default to "No" for safety

    while True:
        stdScreen.clear()

        stdScreen.addstr(5, 5, "WARNING: EXIT GAME?", curses.A_BOLD)
        stdScreen.addstr(6, 5, "Are you sure you want to quit?")

        # Highlight selection
        for i, option in enumerate(options):
            x = 5
            y = 8 + i
            if i == currentSelection:
                stdScreen.attron(curses.A_REVERSE)
                stdScreen.addstr(y, x, f"> {option}")
                stdScreen.attroff(curses.A_REVERSE)
            else:
                stdScreen.addstr(y, x, f"  {option}")

        stdScreen.refresh()
        keyPressed = stdScreen.getch()

        # Handle WASD and Arrow Keys for vertical navigation
        if keyPressed in (curses.KEY_UP, ord("w"), ord("W")):
            currentSelection = max(0, currentSelection - 1)
        elif keyPressed in (curses.KEY_DOWN, ord("s"), ord("S")):
            currentSelection = min(len(options) - 1, currentSelection + 1)
        # Handle ENTER key (10 and 13 cover various terminal environments)
        elif keyPressed in (curses.KEY_ENTER, 10, 13):
            return currentSelection == 0


def _run_main_menu(stdScreen: "curses.window") -> int:
    """
    Draws and handles the interactive main menu.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
    Returns:
      int: The index of the selected option (0 for Play, 1 for Quit).
    """
    options = ["PLAY", "QUIT"]
    currentSelection = 0

    while True:
        stdScreen.clear()

        stdScreen.addstr(2, 5, "TERMINAL PATROLLERS", curses.A_BOLD)
        stdScreen.addstr(
            4, 5, "A fun stealth game mapped directly to real-world city streets!"
        )
        stdScreen.addstr(
            6,
            5,
            "NOTE: Requires an active microservice network connection. "
            "Average match duration is 3 to 5 minutes.",
            curses.A_DIM,
        )

        # Highlight selection
        for i, option in enumerate(options):
            x = 5
            y = 9 + i
            if i == currentSelection:
                stdScreen.attron(curses.A_REVERSE)
                stdScreen.addstr(y, x, f"> {option}")
                stdScreen.attroff(curses.A_REVERSE)
            else:
                stdScreen.addstr(y, x, f"  {option}")

        stdScreen.refresh()
        keyPressed = stdScreen.getch()

        # Handle WASD and Arrow Keys for vertical navigation
        if keyPressed in (curses.KEY_UP, ord("w"), ord("W")):
            currentSelection = max(0, currentSelection - 1)
        elif keyPressed in (curses.KEY_DOWN, ord("s"), ord("S")):
            currentSelection = min(len(options) - 1, currentSelection + 1)

        # Handle ENTER key (10 and 13 cover various terminal environments)
        elif keyPressed in (curses.KEY_ENTER, 10, 13):
            return currentSelection


def _handle_play_placeholder(stdScreen: "curses.window"):
    """
    Displays a placeholder loading screen for the game.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
    Returns:
      None
    """
    stdScreen.clear()
    stdScreen.addstr(5, 5, "Game is loading... (Press any key to return to menu)")
    stdScreen.refresh()
    stdScreen.getch()


def main(stdScreen: "curses.window"):
    """
    Main application loop handling routing and initialization.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
    Returns:
      None
    """
    curses.curs_set(0)  # Hide cursor
    stdScreen.keypad(True)  # Enable escape sequences

    while True:
        # Enter main menu
        selectionIndex = _run_main_menu(stdScreen)

        # 0 for PLAY
        if selectionIndex == 0:
            _handle_play_placeholder(stdScreen)
        # 1 for QUIT
        elif selectionIndex == 1:
            if _run_confirm_exit(stdScreen):
                break


if __name__ == "__main__":
    curses.wrapper(main)
