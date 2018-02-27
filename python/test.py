import pprint
import sys
import spotipy
import spotipy.util as util
import spotipy.client as client
if len(sys.argv) > 3:
  username = sys.argv[1]
  playlist_id = sys.argv[2]
  track_ids = sys.argv[3:]
else:
  print("Usage: %s username playlist_id track_id ..." % (sys.argv[0],))
  sys.exit()

scope = 'user-read-currently-playing user-modify-playback-state'
token = util.prompt_for_user_token(username, scope)

if token:
  sp = spotipy.Spotify(auth=token)
  sp.trace = False
  #results = sp.pause_playback() 
  results = sp.current_user_playing_track()
  if (results['is_playing']):
      print("lol")
  else:
      print(results)
else:
  print("Can't get token for", username)

