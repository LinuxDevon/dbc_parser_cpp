#!/usr/bin/env python3

import pathlib
import subprocess
import os
import unittest

class TestExamples(unittest.TestCase):
    def setUp(self):
        filenames = next(os.walk("./src/"), (None, None, []))[2]
        self.examples = [file for file in filenames if ".cpp" in file]
        self.assertGreaterEqual(len(self.examples), 1)
        self.maxDiff = None  # Enable big diffs of text

    def tearDown(self):
        pass

    def test_hello_world(self):
        for example in self.examples:
            contents = pathlib.Path("src/" + example).read_text()
            example_binary_name = example.split("example_")[1].split(".cpp")[0]
            output = contents.split("=== +++ ===")
            if len(output) < 2:
                print(f"Skipping {example_binary_name} because no output to test.")
                continue

            test_output = output[1].lstrip()
            
            result = subprocess.run("../build/examples/" + example_binary_name, capture_output=True, text=True)
            with self.subTest(msg='Check example run'):
                self.assertEqual(result.returncode, 0)
                self.assertEqual(result.stdout, test_output)
                self.assertEqual(result.stderr, "")

if __name__ == '__main__':
    unittest.main()