// 
// Build project for emscripten
// Run using:
// node make.js [-f FILE, --file=FILE, --makefile=FILE] [target]
//

var target = "DEBUG";

for (var i=2;i<process.argv.length;i++){
    if (process.argv[i].indexOf('-')===0){
        i++;
    } else  if (i === process.argv.length-1){
        target = process.argv[i];
    }
}

var project = {
    "options": [(target=="debug"?"-O0":"-O3"), "-Werror", (target=="debug"?"-s GL_ASSERTIONS=1":""), (target=="debug"?"-s GL_DEBUG=1":"" )/*, "-s USE_SDL=2", "-s USE_SDL_IMAGE=2"*/],
    "buildoptions": ["-std=c++11", "-stdlib=libc++", "-fno-exceptions"],
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
            "filter":"assets/font/.*\\.fnt"
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
            "filter":"assets/font/.*\\.png"
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
