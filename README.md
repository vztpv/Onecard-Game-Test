	
	# Onecard Data and Event.
	
	# Info
	Info = { CARDNUM = 54 } #
	
	# Turn
	Turn = { dir=true start=1 end=4 n=4 now=4 }
	
	Event = {
		# No
		id=1
		# Action
		$if = { $condition = { $EQ = { /Turn/dir true } } 
			$then = { $assign = { /Turn/now { $add = { $modular = { /Turn/now /Turn/n } 1 } } } }
		}	
		$else = { # if else link problem? - depth?, depth max setting?
			$assign = { /Turn/now { $add = { $modular = { $add = { /Turn/now $add = { /Turn/n -2 } } Turn/n}}}}
		}		
	}
	
	# Card
	Event = {
		id = 101
		$parameter = { i } #
		# Action  cf) Card                 <-------------------- 
		$insert = { /Card/ = { sha = { $divide ={$parameter.i 13} } num = { $modular={$parameter.i 13} } # no ???
							isBlackJoker = no isColorJoker = no } }
		$if = { $condition = { $COMP< = { $parameter.i $multiple = { 13 4 }  } }
			$then = { $call = { id = 101 i = { $add = { $parameter.i 1  } } } }
		}
	}
	Event = {
		id = 3
		$call = { id = 101 i = 0 } # using stack? + 몇번쨰까찌 했는가?
		# insert joker
		$insert = { /Card/ = {sha = -1 num = -1  isBlackJoker = yes isColorJoker = no } }
		$insert = { /Card/ = {sha = -2 num = -2  isBlackJoker = no isColorJooker = yes } }
	}
	
	# CardList - RandomShuffle!
	Event =
	{
		id = 4
		$parameter = { i }
		# Action
		$insert = { /CardList/ = { $parameter.i } }
		$if = { $condition = { $COMP< = { $parameter.i /Info/CARDNUM  } }
			$then = { $call = { id = 4 i = { $add = { $parameter.i 1 } } } }
		}            
	}
	Event = 
	{
		id = 5
		# Action
		$call = { id = 4 i = 0 }
	}
	Event = 
	{
		id = 6
		$parameter = { n }
		# Action
		$swap = { /CardList/ $rand{ 0, $add={$parameter.n-1} } $add={$parameter.n-1} }
		$if = { $condition = { $COMP> = { $parameter.n 0 } }
			$then = { $call = { id = 6 n = { $add{ $parameter.n -1 } } } }
		} 
	}
	Event =
	{
		id = 7
		# Action
		$call = { id = 6 n = /Info/CARDNUM } 
	}
	
	# PutCard
	
	
	# Rule
	
	
	# AttackPoint
	
	
	# State
	
	
	# Person # cf) inheritance?
	
	
	# Computer
	
	
	# Print
	
	
	# Total?
	Event =
	{
		id = 0	
		
		#Action
		$call = { id = 3 }
		$call = { id = 7 }
	}
	
	# Main
	Main =
	{
		$call = { id = 0 }
	
		# $on = { id = ERROR }
		# $on = { id = CONSOLE }
	}
	
	# Event : Evemt1 -> Event2 -> Event3 -> Event4
	#					-> Event2 or Event5 or Event1 or Event(for Error)?
	#
