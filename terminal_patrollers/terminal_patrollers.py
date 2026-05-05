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


def _is_valid_move(mapGrid: list[list[str]], targetY: int, targetX: int) -> bool:
    """
    Checks if the target coordinate is a valid movement space.

    Parameters:
      mapGrid (list[list[str]]): The map matrix.
      targetY (int): Target row index.
      targetX (int): Target column index.
    Returns:
      bool: True if the move is valid, False otherwise.
    """
    maxY = len(mapGrid)
    maxX = len(mapGrid[0]) if maxY > 0 else 0

    # Compress conditionals for bounds and wall collisions
    inBounds = 0 <= targetY < maxY and 0 <= targetX < maxX

    return inBounds and mapGrid[targetY][targetX] != "#"


def _calculate_new_pos(
    keyPressed: int, currentY: int, currentX: int
) -> tuple[int, int]:
    """
    Calculates the new coordinates based on the movement key pressed.

    Parameters:
      keyPressed (int): The ASCII or curses key code.
      currentY (int): The current row coordinate.
      currentX (int): The current column coordinate.
    Returns:
      tuple[int, int]: The potentially updated (Y, X) coordinates.
    """
    newY = currentY
    newX = currentX

    # W or Arrow Up
    if keyPressed in (curses.KEY_UP, ord("w"), ord("W")):
        newY -= 1
    # S or Arrow Down
    elif keyPressed in (curses.KEY_DOWN, ord("s"), ord("S")):
        newY += 1
    # A or Arrow Left
    elif keyPressed in (curses.KEY_LEFT, ord("a"), ord("A")):
        newX -= 1
    # D or Arrow Right
    elif keyPressed in (curses.KEY_RIGHT, ord("d"), ord("D")):
        newX += 1

    return newY, newX


def _load_fake_map() -> list[list[str]]:
    """
    Generates a static 2D grid representing a city map.
    This is intended to be a placeholder map.

    Parameters:
      None
    Returns:
      list[list[str]]: A 2D list of map characters.
    """
    rawMap = [
        "...........................",
        "....#######.....#######....",
        "....#######.....#######....",
        "...........................",
        "....#######................",
        "....#######.......#####....",
        "..................#####....",
        "...........................",
    ]

    return [list(row) for row in rawMap]


def _draw_map(
    stdScreen: "curses.window", mapGrid: list[list[str]], playerY: int, playerX: int
):
    """
    Iterates through the map matrix and renders it on screen.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
      mapGrid (list[list[str]]): The 2D array of map characters.
      playerY (int): The player's row coordinate.
      playerX (int): The player's column coordinate.
    Returns:
      None
    """
    mapHeight = len(mapGrid)
    mapWidth = len(mapGrid[0]) if mapHeight > 0 else 0

    startY, startX = _get_centered_coords(stdScreen, mapWidth, -(mapHeight // 2))

    for rIndex, row in enumerate(mapGrid):
        for cIndex, char in enumerate(row):
            # Conditionally render the player avatar
            drawChar = "@" if rIndex == playerY and cIndex == playerX else char
            stdScreen.addstr(startY + rIndex, startX + cIndex, drawChar)


def _run_game_loop(stdScreen: "curses.window"):
    """
    Handles the active game view state and map rendering.

    Parameters:
      stdScreen ("curses.window"): The standard screen object.
    Returns:
      None
    """
    mapGrid = _load_fake_map()
    playerY = 0
    playerX = 0

    while True:
        stdScreen.clear()
        _draw_map(stdScreen, mapGrid, playerY, playerX)

        # Movement info in the status bar
        navText = "Move: [W][A][S][D] or [ARROW KEYS]"
        y, x = _get_centered_coords(stdScreen, len(navText), (len(mapGrid) // 2) + 2)
        stdScreen.addstr(y, x, navText, curses.A_DIM)

        # Escape instructions in the status bar
        # Placeholder until pause menu implemented
        instruction = "Press [ESC] to return to menu"
        y, x = _get_centered_coords(
            stdScreen, len(instruction), (len(mapGrid) // 2) + 3
        )
        stdScreen.addstr(y, x, instruction, curses.A_DIM)

        stdScreen.refresh()
        keyPressed = stdScreen.getch()

        # 27 is the standard ASCII code for the Escape key
        if keyPressed == 27:
            break
        elif keyPressed == curses.KEY_RESIZE:
            curses.update_lines_cols()
        else:
            # Calculate and validate player movement
            newY, newX = _calculate_new_pos(keyPressed, playerY, playerX)
            if _is_valid_move(mapGrid, newY, newX):
                playerY = newY
                playerX = newX


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
            _run_game_loop(stdScreen)
        # 1 for QUIT
        elif selectionIndex == 1:
            if _run_confirm_exit(stdScreen):
                break


if __name__ == "__main__":
    curses.wrapper(main)
