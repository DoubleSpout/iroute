{
  "targets":[
    {
      "target_name": "iroute",
      "conditions": [
            ["OS==\"mac\"", {  
                              "sources": ["src/iroute.cpp","src/route.cpp"],
                               "libraries": [],
                               "cflags": ["-g", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE", "-Wall", "-O0", "-Wunused-macros"],
            }],
            ["OS==\"linux\"", {
                               "sources": ["src/iroute.cpp","src/route.cpp"],
                               "libraries": [],
                               "cflags": [],
            }],
            ["OS==\"win\"", {  
	                 "sources": ["src/iroute.cpp","src/route.cpp"],
			 "libraries": [],
                         "cflags": ["/TP",'/J','/E'],
            }]
        ]
    }
  ]
}
