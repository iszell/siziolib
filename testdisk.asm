	.plugin "se.booze.kickass.CruncherPlugins"

	.encoding	"petscii_mixed"

	.disk [filename="testdisk.d64", name="IOLIBV3TEST", id="2021!" ]
	{
		[name="DEMO", type="prg", segments="demo" ],
		[name="IOLIBV3TEST", type="prg", segments="iolibv3test" ],
		[name="IOLIBV3EXOTEST", type="prg", segments="iolibv3exotest" ],
		[name="TESTPATTERN", type="prg", segments="testdata" ],
		[name="B1", type="prg", segments="bitmapdata" ],
		[name="E1", type="prg", segments="bitmapexodata" ],
		[name="EXOTESTDATA", type="prg", segments="exotestdata" ]
	}

	.segment demo []
	* = $1001
	.import c64 "demo.prg"

	.segment iolibv3test []
	* = $1001
	.import c64 "IOLibV3Test.prg"

	.segment iolibv3exotest []
	* = $1001
	.import c64 "IOLibV3ExoTest.prg"

	.segment testdata []
	* = $3000
	.for(var h=$30 ; h<$f0 ; h++) {
		.for(var l=0 ; l<256 ; l++) .byte l
	}

	.segment bitmapdata []
		* = $2000
		.import c64 "bitmap1.bin"

	.segment bitmapexodata []
	.modify ForwardMemExomizer($2000) {
		* = $2000
		.import c64 "bitmap1.bin"
	}

	.segment exotestdata []
	.modify ForwardMemExomizer($5800) {
		* = $5800
		.import c64 "dotctitle.bin"
	}
