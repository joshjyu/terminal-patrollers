import curses


def _get_centered_coords(
    stdScreen: "curses.window", textLen: int, offsetY: int = 0
) -> tuple[int, int]:
    """
    Calculates the Y and X coordinates to center text on the screen.

    Parameters:
      stdScreen ("curses.window"): The window object.
      textLen (int): Length of the text to be centered.
      offsetY (int): Vertical offset from the absolute center.
    Returns:
      tuple[int, int]: The calculated (y, x) coordinates.
    """
    maxY, maxX = stdScreen.getmaxyx()
    x = (maxX - textLen) // 2
    y = (maxY // 2) + offsetY

    return y, x


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

        warnTitle = "WARNING: EXIT GAME?"
        warnDesc = "Are you sure you want to quit?"

        # Add exit confirmation text
        y, x = _get_centered_coords(stdScreen, len(warnTitle), -2)
        stdScreen.addstr(y, x, warnTitle, curses.A_BOLD)
        y, x = _get_centered_coords(stdScreen, len(warnDesc), -1)
        stdScreen.addstr(y, x, warnDesc)

        # Render options and highlight selections
        for i, option in enumerate(options):
            label = f"> {option}" if i == currentSelection else f"  {option}"
            y, x = _get_centered_coords(stdScreen, len(label), 1 + i)

            if i == currentSelection:
                stdScreen.attron(curses.A_REVERSE)
                stdScreen.addstr(y, x, label)
                stdScreen.attroff(curses.A_REVERSE)
            else:
                stdScreen.addstr(y, x, label)

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

        # Handle terminal window resizing
        elif keyPressed == curses.KEY_RESIZE:
            curses.update_lines_cols()


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

        title = "TERMINAL PATROLLERS"
        desc = "A fun stealth game mapped directly to real-world city streets!"
        costNote = "NOTE: Requires an active microservice network connection."

        # Add title
        y, x = _get_centered_coords(stdScreen, len(title), -4)
        stdScreen.addstr(y, x, title, curses.A_BOLD)

        # Add description
        y, x = _get_centered_coords(stdScreen, len(desc), -2)
        stdScreen.addstr(y, x, desc)

        # Add cost note
        y, x = _get_centered_coords(stdScreen, len(costNote), 0)
        stdScreen.addstr(y, x, costNote, curses.A_DIM)

        # Render options and highlight selections
        for i, option in enumerate(options):
            label = f"> {option}" if i == currentSelection else f"  {option}"
            y, x = _get_centered_coords(stdScreen, len(label), 3 + i)

            if i == currentSelection:
                stdScreen.attron(curses.A_REVERSE)
                stdScreen.addstr(y, x, label)
                stdScreen.attroff(curses.A_REVERSE)
            else:
                stdScreen.addstr(y, x, label)

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

        # Handle terminal window resizing
        elif keyPressed == curses.KEY_RESIZE:
            curses.update_lines_cols()


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
