#include <catch2/catch_test_macros.hpp>

#include "Dialogs.hpp"

#include "xplorer/model/XpanderConstants.hpp"

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

// The stored name is fixed-width: XpanderTone::setToneName space-pads it to
// TONE_NAME_LENGTH because the padding belongs to the wire format
// (RQ-MOD-023). Pre-filling the rename field with that form spent part of the
// same 8-character budget the input filter enforces on characters the user
// cannot see, so a 6-character patch could only be renamed with 6 new
// characters. [RQ-GUI-081]
SCENARIO("The rename field is pre-filled without the stored name's wire padding",
         "[RQ-GUI-081]")
{
    GIVEN("a stored name shorter than the fixed width, space-padded to it")
    {
        const std::string stored = "BRASS   "; // 5 significant + 3 pad

        THEN("the editable form carries the significant characters only")
        {
            REQUIRE(patchNameForEditing(stored) == "BRASS");
        }

        THEN("the whole length budget is left for the user to type")
        {
            const auto typed = patchNameForEditing(stored).size();
            REQUIRE(static_cast<int>(xplorer::model::constants::TONE_NAME_LENGTH - typed) == 3);
            REQUIRE(patchNameForEditing(stored).size()
                    < static_cast<std::size_t>(xplorer::model::constants::TONE_NAME_LENGTH));
        }
    }

    GIVEN("a six-character stored name -- the reported defect")
    {
        THEN("two characters of budget are recovered, for eight in total")
        {
            const auto editable = patchNameForEditing("STRING  ");
            REQUIRE(editable == "STRING");
            REQUIRE(static_cast<int>(editable.size()) + 2
                    == xplorer::model::constants::TONE_NAME_LENGTH);
        }
    }

    GIVEN("a stored name that fills the fixed width")
    {
        THEN("it is left untouched")
        {
            REQUIRE(patchNameForEditing("12345678") == "12345678");
        }
    }

    GIVEN("a stored name whose leading and interior spaces are the user's own")
    {
        THEN("only the trailing padding is removed")
        {
            REQUIRE(patchNameForEditing(" LEAD 1 ") == " LEAD 1");
        }
    }

    GIVEN("a stored name that is nothing but padding")
    {
        THEN("the field opens empty")
        {
            REQUIRE(patchNameForEditing("        ").empty());
        }
    }

    GIVEN("the editable form of any stored name")
    {
        THEN("it is still a valid patch name")
        {
            REQUIRE(isPatchNameValid(patchNameForEditing("BRASS   ")));
            REQUIRE(isPatchNameValid(patchNameForEditing("        ")));
            REQUIRE(isPatchNameValid(patchNameForEditing("A</>+-*$")));
        }
    }
}
