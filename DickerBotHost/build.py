import os
import platform
import shutil

def build():
    system = platform.system()

    # Clean old build/dist folders first
    if os.path.exists('build'):
        shutil.rmtree('build')
    if os.path.exists('dist'):
        shutil.rmtree('dist')

    # Common base command
    cmd = 'pyinstaller --noconfirm --onefile --windowed '

    # Add Host.ui resource
    if system == "Darwin":
        cmd += '--name "DickerBotHost" '
        cmd += '--add-data "Host.ui:." '
        cmd += '--icon dickerbot.icns '
    elif system == "Windows":
        cmd += '--name "DickerBotHost" '
        cmd += '--add-data "Host.ui;." '
        cmd += '--icon dickerbot.ico '
    else:
        print(f"Unsupported OS: {system}")
        return

    # Final part of the command (the script name)
    cmd += 'Host.py'

    # Run it
    os.system(cmd)

if __name__ == "__main__":
    build()
