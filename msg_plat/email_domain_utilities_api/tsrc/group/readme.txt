
Short guide to STIF:
Because ImumDomainApiTest test cases require CoeEnv support, these tests are implemented by using TestScripter class template.
This is the only way to make ui support work. 


With STIF and this particular template one must remember two things:
* TestFramework.ini defines which test.dll´s OR test.cfg´s are run
* .cfg defines how the test methods are run and how they are shown in STIF UI


When defining a new test method:
1. Write a new method as you would with any other testframework.
2. Update CImumDomainApiTest::RunMethodL() with your new test method
 * ENTRY( <name of test method> , <test method> )
 * When defining description do NOT use empty spaces 
3. Update ui_ImumDomainApiTest.cfg under group folder
 * Copy-paste one [Test]-[EndTest] paragraph
 * Give a describing name of the case next to 'title'. This line is shown in STIF UI when running cases. You can use spaces.
 * Give name of the test method next to 'test'. This name <name of test method> has to be the same which was defined previously.
4. Run copy_settings_files_to_platform.bat to move .cfg file to correct place
 * or manually copy it to \epoc32\winscw\c\TestFramework
5. Run tests by using STIF UI under RnD Tools.