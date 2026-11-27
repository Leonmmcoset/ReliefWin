# PROJECT:     ReliefWin API tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Verify driver database records for null-service volume installation
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import pathlib
import subprocess
import sys
import tempfile


def main():
    binary = pathlib.Path(sys.argv[1]).resolve()
    source = pathlib.Path(__file__).resolve().parents[4] / "media/inf/volume.inf"
    with tempfile.TemporaryDirectory() as directory:
        root = pathlib.Path(directory)
        listing = root / "inputs.txt"
        listing.write_text(str(source) + "\n")
        output = root / "database.inf"
        subprocess.run([str(binary), "amd64", str(output), str(listing)], check=True)
        lines = output.read_text().splitlines()
        service = [line for line in lines if '\\Configurations\\VolumeInstall","Service"' in line]
        assert len(service) == 1, service
        assert service[0].endswith(',0x00000000,""'), service[0]
        assert not any('\\Services\\"' in line for line in lines)
    print("null service: 3 checks passed")


if __name__ == "__main__":
    main()
