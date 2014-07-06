// 
// Build project for emscripten
// Run using:
// node make.js [-f FILE, --file=FILE, --makefile=FILE] [target]
//

var target = "DEBUG";

for (var i=2;i<process.argv.length;i++){
    console.log(process.argv[i]);
    if (process.argv[i].indexOf('-')===0){
        i++;
    } else  if (i === process.argv.length-1){
        target = process.argv[i];
    }
}

var project = {
    "options": [(target=="debug"?"-O0":"-O3"), "-Werror"/*, "-s STB_IMAGE=1"*/],
    "buildoptions": ["-std=c++11", "-stdlib=libc++"],
    "defines": ["GLM_FORCE_RADIANS",(target=="debug"?"DEBUG":"NDEBUG")],
    "targetdir": "../build-emscripten",
    "target": "kickcpp.html",
    "includedirs": [ "src", "unittest", "libs/include" ],
	"files":[ {
            "root":".",
            "filter":"src/.*\\.cpp"
        },
        {
            "root":".",
            "filter":"unittest/.*\\.cpp"}],
    "embed":[
		{
            // default is "."
			"root":".",
            // matcher pattern
			"filter":"assets/shaders/\\.*"
		},
        {
            // default is "."
            "root":".",
            // matcher pattern
            "filter":"unittest/testasset/\\.*"
        },
        {
            // default is "."
            "root":".",
            // matcher pattern
            "filter":"unittest/font/.*\\.fnt"
        },
        {
            // default is "."
            "root":".",
            // matcher pattern
            "filter":"unittest/sprites/.*\\.txt"
        }],
    "preload":[
        {
            // default is "."
            "root":".",
            // matcher pattern
            "filter":"assets/textures/\\.*"
        },
        {
            // default is "."
            "root":".",
            // matcher pattern
            "filter":"unittest/images/\\.*"
        },
        {
            // default is "."
            "root":".",
            // matcher pattern
            "filter":"unittest/font/.*\\.png"
        },
        {
            // default is "."
            "root":".",
            // matcher pattern
            "filter":"unittest/sprites/.*\\.png"
        }
    ]
};

module.exports = project;
