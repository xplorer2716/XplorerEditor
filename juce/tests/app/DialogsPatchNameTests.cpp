#include <catch2/catch_test_macros.hpp>

#include "Dialogs.hpp"

// isPatchNameValid gates what the rename dialog will accept on submit -- the
// same character set and length now also enforced live via the TextEditor's
// InputFilter (PatchNameInputFilter, file-local to Dialogs.cpp). Reference
// RenamePatchForm.Designer.cs: tbxPatchName.MaxLength = 8,
// CharacterCasing.Upper; RenamePatchForm.IsPatchNameValid's validChars.
// [RQ-GUI-025]

using namespace xplorer::app;

SCENARIO("A patch name is valid only within the XPANDER's character set and length",
         "[RQ-GUI-025]")
{
    GIVEN("a name using only allowed characters, within the 8-character limit")
    {
        THEN("it is valid")
        {
            REQUIRE(isPatchNameValid("XPLORER"));
            REQUIRE(isPatchNameValid("LEAD 1"));
        }
    }

    GIVEN("a name using every allowed punctuation character")
    {
        THEN("it is valid")
        {
            REQUIRE(isPatchNameValid("A</>+-*$"));
        }
    }

    GIVEN("a name exactly at the 8-character limit")
    {
        THEN("it is valid")
        {
            REQUIRE(isPatchNameValid("12345678"));
        }
    }

    GIVEN("a name longer than the 8-character limit")
    {
        THEN("it is invalid, even though every character is individually allowed")
        {
            REQUIRE_FALSE(isPatchNameValid("123456789"));
        }
    }

    GIVEN("a name containing a lowercase letter")
    {
        THEN("it is invalid")
        {
            REQUIRE_FALSE(isPatchNameValid("Lead"));
        }
    }

    GIVEN("a name containing a character outside the reference set")
    {
        THEN("it is invalid")
        {
            REQUIRE_FALSE(isPatchNameValid("LEAD#1"));
            REQUIRE_FALSE(isPatchNameValid("CAF\xC3\xA9")); // UTF-8 'CAFÉ'
        }
    }

    GIVEN("the empty name")
    {
        THEN("it is valid, matching the reference's own IsPatchNameValid")
        {
            REQUIRE(isPatchNameValid(""));
        }
    }
}
