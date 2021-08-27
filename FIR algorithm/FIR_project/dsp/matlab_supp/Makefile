all: audio_play.dll audio_record.dll audio_recplay.dll rtdx.dll

audio_play.dll: audio_play.cpp audio.cpp
	mex.bat -DINITGUID audio_play.cpp audio.cpp Dsound.lib gdi32.lib user32.lib
	cp -f audio_play.dll ../../mex

audio_record.dll: audio_record.cpp audio.cpp
	mex.bat -DINITGUID audio_record.cpp audio.cpp Dsound.lib gdi32.lib user32.lib
	cp -f audio_record.dll ../../mex

audio_recplay.dll: audio_recplay.cpp audio.cpp
	mex.bat -DINITGUID audio_recplay.cpp audio.cpp Dsound.lib gdi32.lib user32.lib
	cp -f audio_recplay.dll ../../mex

rtdx.dll: rtdx.cpp
	mex.bat rtdx.cpp
	cp -f rtdx.dll ../../mex

clean:
	rm -f *.dll *.mexw32
