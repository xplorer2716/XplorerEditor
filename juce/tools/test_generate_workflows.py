#!/usr/bin/env python3
"""Unit tests for generate_workflows.py's macOS Debug canary screenshot step.

Guards the scope guard added for RQ-BLD-032 (ADR-BLD-006, DEC-BLD-029): the
generated screenshot-macos-app call must appear for the macOS/Debug/canary
combination only, and for no other platform/config/stream combination the
matrix produces.

Run:  python3 juce/tools/test_generate_workflows.py
"""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import generate_workflows as gw  # noqa: E402

SCREENSHOT_MARKER = "./.github/actions/screenshot-macos-app"


class ScreenshotStepScopeTests(unittest.TestCase):
    """RQ-BLD-032: the launch-screenshot step is generated for macOS Debug canary only."""

    def test_macos_debug_canary_includes_screenshot_step(self):
        # Given the macOS/arm64/Debug/canary combination
        # When its workflow body is generated
        _, body = gw.workflow("macos", "arm64", "macos-latest", "debug", "canary")
        # Then it calls the screenshot-macos-app action
        self.assertIn(SCREENSHOT_MARKER, body)

    def test_every_other_combination_excludes_screenshot_step(self):
        # Given every platform/config/stream combination the matrix produces
        for os_name, arch, runner in gw.PLATFORMS:
            for stage, (configs, _, _) in gw.STREAMS.items():
                for config in configs:
                    if os_name == "macos" and config == "debug" and stage == "canary":
                        continue  # the one combination expected to carry it
                    with self.subTest(os=os_name, config=config, stage=stage):
                        # When its workflow body is generated
                        _, body = gw.workflow(os_name, arch, runner, config, stage)
                        # Then it carries no screenshot step
                        self.assertNotIn(SCREENSHOT_MARKER, body)


if __name__ == "__main__":
    unittest.main()
