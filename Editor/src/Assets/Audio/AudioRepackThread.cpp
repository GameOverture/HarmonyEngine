/**************************************************************************
 *	AudioRepackThread.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioRepackThread.h"

#include <vorbis/vorbisenc.h>

AudioRepackThread::AudioRepackThread(QMap<BankData *, QSet<IAssetItemData *>> &affectedAssetsMapRef, QDir metaDir) :
	IRepackThread(affectedAssetsMapRef, metaDir)
{
}

/*virtual*/ AudioRepackThread::~AudioRepackThread()
{
}

/*virtual*/ void AudioRepackThread::OnRun() /*override*/
{
	for(auto iter = m_AffectedAssetsMapRef.begin(); iter != m_AffectedAssetsMapRef.end(); ++iter)
	{
		BankData *pBank = iter.key();
		QSet<IAssetItemData *> affectedSet(iter.value());

		// First remove any stale audio files that are missing in 'pBank->m_AssetList'
		QDir runtimeBankDir(pBank->m_sAbsPath);
		QFileInfoList existingAudioInfoList = runtimeBankDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
		for(auto fileInfo : existingAudioInfoList)
		{
			bool bFound = false;
			for(auto audio : pBank->m_AssetList)
			{
				if(fileInfo.baseName().compare(static_cast<AudioAsset *>(audio)->ConstructDataFileName(false), Qt::CaseInsensitive) == 0)
				{
					// If this asset is in affected set, delete it as it will be repacked
					for(auto affected : affectedSet)
					{
						if(audio == affected)
						{
							QFile::remove(fileInfo.absoluteFilePath());
							break;
						}
					}

					bFound = true;
					break;
				}
			}

			if(bFound == false)
				QFile::remove(fileInfo.absoluteFilePath());
		}

		// Repack all audio in affectedSet
		for(auto audio : affectedSet)
		{
			AudioAsset *pAudioAsset = static_cast<AudioAsset *>(audio);

			if(pAudioAsset->IsCompressed())
				PackToOgg(pAudioAsset, runtimeBankDir);
			else
				PackToWav(pAudioAsset, runtimeBankDir);
		}
	}
}

bool AudioRepackThread::PackToWav(AudioAsset *pAudio, QDir runtimeBankDir)
{
	// TODO: convert WAV to target format
	return QFile::copy(m_MetaDir.absoluteFilePath(pAudio->ConstructMetaFileName()),
					   runtimeBankDir.absoluteFilePath(pAudio->ConstructDataFileName(true)));
}

bool AudioRepackThread::PackToOgg(AudioAsset *pAudio, QDir runtimeBankDir)
{
	QString sWavFilePath = m_MetaDir.absoluteFilePath(pAudio->ConstructMetaFileName());
	QFile wavFile(sWavFilePath);
	if(!wavFile.open(QIODevice::ReadOnly))
	{
		HyGuiLog("WAV file '" % sWavFilePath % "' not able to be opened: " % wavFile.errorString(), LOGTYPE_Error);
		return false;
	}
	int dataLength = wavFile.size() - 44;
	
	QString sOggFilePath = runtimeBankDir.absoluteFilePath(pAudio->ConstructDataFileName(true));
	QFile oggFile(sOggFilePath);
	if(!oggFile.open(QIODevice::WriteOnly))
	{
		HyGuiLog("OGG file '" % sOggFilePath % "' not able to be opened: " % oggFile.errorString(), LOGTYPE_Error);
		return false;
	}
	//FILE *outFile = _fdopen(_dup(oggFile.handle()), "wb");



	//signed char *readbuffer;//[READ*4+44];
	//readbuffer = new signed char[dataLength + 44];

	ogg_stream_state	os; // take physical pages, weld into a logical stream of packets
	ogg_page			og; // one Ogg bitstream page.  Vorbis packets are inside
	ogg_packet			op; // one raw packet of data for decode

	vorbis_info			vi; // struct that stores all the static vorbis bitstream settings
	vorbis_comment		vc; // struct that stores all the user comments
	vorbis_dsp_state	vd; // central working state for the packet->PCM decoder
	vorbis_block		vb; // local working space for packet->PCM decode

	int eos = 0, ret;
	//int i, founddata;

	//#if defined(macintosh) && defined(__MWERKS__)
	//    int argc = 0;
	//    char **argv = NULL;
	//    argc = ccommand(&argv); /* get a "command line" from the Mac user */
	//    /* this also lets the user set stdin and stdout */
	//#endif

	/* we cheat on the WAV header; we just bypass 44 bytes (simplest WAV
	 header is 44 bytes) and assume that the data is 44.1khz, stereo, 16 bit
	 little endian pcm samples. This is just an example, after all. */

	 //#ifdef _WIN32 /* We need to set stdin/stdout to binary mode. Damn windows. */
	 //    /* if we were reading/writing a file, it would also need to in
	 //     binary mode, eg, fopen("file.wav","wb"); */
	 //    /* Beware the evil ifdef. We avoid these where we can, but this one we
	 //     cannot. Don't add any more, you'll probably go to hell if you do. */
	 //    _setmode( _fileno( stdin ), _O_BINARY );
	 //    _setmode( _fileno( stdout ), _O_BINARY );
	 //#endif


	 /* we cheat on the WAV header; we just bypass the header and never
	  verify that it matches 16bit/stereo/44.1kHz.  This is just an
	  example, after all. */

	


	//readbuffer[0] = '\0';
	//for(i = 0, founddata = 0; i < 30 && !feof(inFile) && !ferror(inFile); i++)
	//{
	//	fread(readbuffer, 1, 2, inFile);

	//	if(!strncmp((char *)readbuffer, "da", 2))
	//	{
	//		founddata = 1;
	//		fread(readbuffer, 1, 6, inFile);
	//		break;
	//	}
	//}

	/********** Encode setup ************/

	

	/* choose an encoding mode.  A few possibilities commented out, one
	 actually used: */

	 /*********************************************************************
	Encoding using a VBR quality mode.  The usable range is -.1
	(lowest quality, smallest file) to 1. (highest quality, largest file).
	Example quality mode .4: 44kHz stereo coupled, roughly 128kbps VBR
	ret = vorbis_encode_init_vbr(&vi,2,44100,.4);
	---------------------------------------------------------------------
	Encoding using an average bitrate mode (ABR).
	example: 44kHz stereo coupled, average 128kbps VBR
	ret = vorbis_encode_init(&vi,2,44100,-1,128000,-1);
	---------------------------------------------------------------------
	Encode using a quality mode, but select that quality mode by asking for
	an approximate bitrate.  This is not ABR, it is true VBR, but selected
	using the bitrate interface, and then turning bitrate management off:
	ret = ( vorbis_encode_setup_managed(&vi,2,44100,-1,128000,-1) ||
			vorbis_encode_ctl(&vi,OV_ECTL_RATEMANAGE2_SET,NULL) ||
			vorbis_encode_setup_init(&vi));
	*********************************************************************/
	WaveHeader wavHeader;
	wavFile.read(reinterpret_cast<char *>(&wavHeader), sizeof(WaveHeader));

	vorbis_info_init(&vi);
	ret = vorbis_encode_init_vbr(&vi, wavHeader.NumOfChan, wavHeader.SamplesPerSec, HyMath::Clamp(pAudio->GetVbrQuality(), 0.0, 1.0));
	if(ret)
	{
		// do not continue if setup failed; this can happen if we ask for a
		// mode that libVorbis does not support (eg, too low a bitrate, etc,
		// will return 'OV_EIMPL')
		HyGuiLog("vorbis_encode_init_vbr failed: " % QString::number(ret), LOGTYPE_Error);
		return false;
	}

	vorbis_comment_init(&vc);
	vorbis_comment_add_tag(&vc, "Harmony", "Editor Converted");

	// set up the analysis state and auxiliary encoding storage
	vorbis_analysis_init(&vd, &vi);
	vorbis_block_init(&vd, &vb);

	// set up our packet->stream encoder
	// pick a random serial number; that way we can more likely build chained streams just by concatenation
	srand(time(NULL));
	ogg_stream_init(&os, rand());

	// Vorbis streams begin with three headers; the initial header (with
	// most of the codec setup parameters) which is mandated by the Ogg
	// bitstream spec. The second header holds any comment fields. The
	// third header holds the bitstream codebook. We merely need to
	// make the headers, then pass them to libvorbis one at a time;
	// libvorbis handles the additional Ogg bitstream constraints
	{
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;

		vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
		ogg_stream_packetin(&os, &header); // automatically placed in its own page
		ogg_stream_packetin(&os, &header_comm);
		ogg_stream_packetin(&os, &header_code);

		// This ensures the actual audio data will start on a new page, as per spec
		while(true)
		{
			int result = ogg_stream_flush(&os, &og);
			if(result == 0) // all packet data has already been flushed into pages, and there are no packets to put into the page.
				break;

			oggFile.write(reinterpret_cast<char *>(og.header), og.header_len);
			//fwrite(og.header, 1, og.header_len, outFile);

			oggFile.write(reinterpret_cast<char *>(og.body), og.body_len);
			//fwrite(og.body, 1, og.body_len, outFile);
		}
	}

	const int iREAD_CHUNK = 1024;

	while(!eos)
	{
		long i;
		//long bytes = fread(readbuffer, 1, dataLength, inFile); /* stereo hardwired here */
		QByteArray wavData = wavFile.read(iREAD_CHUNK * 4);

		if(wavData.isEmpty())// bytes == 0) // end of file.
		{
			// this can be done implicitly in the mainline,
			// but it's easier to see here in non-clever fashion.
			// Tell the library we're at end of stream so that it can handle
			// the last frame and mark end of stream in the output properly
			vorbis_analysis_wrote(&vd, 0);
		}
		else // More data to encode
		{
			// expose the buffer to submit data
			float **buffer = vorbis_analysis_buffer(&vd, iREAD_CHUNK);

			///* uninterleave samples */
			//for(i = 0; i < bytes / 4; i++)
			//{
			//	buffer[0][i] = ((readbuffer[i * 4 + 1] << 8) | (0x00ff & (int)readbuffer[i * 4])) / 32768.f;
			//	buffer[1][i] = ((readbuffer[i * 4 + 3] << 8) | (0x00ff & (int)readbuffer[i * 4 + 2])) / 32768.f;
			//}
			
			if(wavHeader.NumOfChan == 2 && pAudio->IsExportMono())
			{
				//Stereo input, mono output
				for(i = 0; i < wavData.size() / 4; i++)
				{
					//Get left and right channels and half them
					buffer[0][i] = ((((wavData[i * 4 + 1] << 8) | (0x00ff & (int)wavData[i * 4])) / 32768.f) + (((wavData[i * 4 + 3] << 8) | (0x00ff & (int)wavData[i * 4 + 2])) / 32768.f)) * 0.5f;
				}
			}
			else if(wavHeader.NumOfChan == 2 && pAudio->IsExportMono() == false)
			{
				//deinterleave samples
				for(i = 0; i < wavData.size() / 4; i++)
				{
					buffer[0][i] = ((wavData[i * 4 + 1] << 8) | (0x00ff & (int)wavData[i * 4])) / 32768.f;
					buffer[1][i] = ((wavData[i * 4 + 3] << 8) | (0x00ff & (int)wavData[i * 4 + 2])) / 32768.f;
				}
			}
			else if(wavHeader.NumOfChan == 1 && pAudio->IsExportMono() == false)
			{
				//get the mono channel and add it to both left and right
				for(i = 0; i < wavData.size() / 2; i++)
				{
					float monoChl = ((wavData[i * 2 + 1] << 8) | (0x00ff & (int)wavData[i * 2])) / 32768.f;
					buffer[0][i] = monoChl;
					buffer[1][i] = monoChl;
				}
			}
			else if(wavHeader.NumOfChan == 1 && pAudio->IsExportMono())
			{
				//get the mono channel
				for(i = 0; i < wavData.size() / 2; i++)
				{
					buffer[0][i] = ((wavData[i * 2 + 1] << 8) | (0x00ff & (int)wavData[i * 2])) / 32768.f;
				}
			}
			else
			{
				HyGuiLog("WavToOgg: The input wav channels '" % QString::number(wavHeader.NumOfChan) % "' cannot encode to " % (pAudio->IsExportMono() ? "mono" : "stereo") % " ogg", LOGTYPE_Error);
				return false;
			}

			// tell the library how much we actually submitted
			vorbis_analysis_wrote(&vd, i);
		}

		// vorbis does some data preanalysis, then divvies up blocks for
		// more involved (potentially parallel) processing. Get a single
		// block for encoding now
		while(vorbis_analysis_blockout(&vd, &vb) == 1)
		{
			// analysis, assume we want to use bitrate management
			vorbis_analysis(&vb, NULL);
			vorbis_bitrate_addblock(&vb);

			while(vorbis_bitrate_flushpacket(&vd, &op))
			{
				// weld the packet into the bitstream
				ogg_stream_packetin(&os, &op);

				// write out pages (if any)
				while(!eos)
				{
					int result = ogg_stream_pageout(&os, &og);
					if(result == 0)
						break;

					//fwrite(og.header, 1, og.header_len, outFile);
					oggFile.write(reinterpret_cast<char *>(og.header), og.header_len);

					//fwrite(og.body, 1, og.body_len, outFile);
					oggFile.write(reinterpret_cast<char *>(og.body), og.body_len);

					// this could be set above, but for illustrative purposes, I do
					// it here (to show that vorbis does know where the stream ends)
					if(ogg_page_eos(&og))
						eos = 1;
				}
			}
		}
	}

	/* clean up and exit.  vorbis_info_clear() must be called last */

	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);

	/* ogg_page and ogg_packet structs always point to storage in
	 libvorbis.  They're never freed or manipulated directly */

	//fclose(inFile);
	wavFile.close();
	//fclose(outFile);
	oggFile.close();

	return true;
}
