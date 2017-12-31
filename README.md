# Using VisualStudio 2017(C++17 option), Windows10, Modern C++

# What is ClauText? 
	ClauText consists of Data, Event, and Main.
		Data consists of =, {, }, String.
			String consists of all character( including digit ) except #, white space characters, and some characters. 
			( but "#1" is allowed. and also " test wow" is allowed. )
		Data example
			dlc = { "power of usa" "power of rus" }
			country = {
				0 = { 
					name = USA 
					power = 100 
					province = { 0 1 2 3 4 5 }
				} 
				1 = { 
					name = RUS 
					power = 90 
					province = { 6 7 8 9 10 11 }
				}
			}
		Event ( function? ) also consists of =, {, }, String.
		Event example
			Event =
			{
				id = 0
				$call = { id = 1 i = 1 j = 1 }
			}
			Event =
			{
				id = 1

				$option = { REMOVE_IF_CALL_ONESELF_EVENT } # REMOVE_IF_CALL_ONESELF_EVENT.
				$parameter = { i j }
				$local = { x } # "NOT_DEFINED"??

				$assign_local = {
					dir = { $local.x }
					data = { 9 }
				}
				$assign_local = {
					dir = { $concat = { $ local.x } } 
					data = { 3 }
				}
				$if = {
					condition = { $COMP<EQ = { $parameter.i $local.x } }
					then = {
						$call = { id = { $concat = { x $add = { 1 1 } } } i = $parameter.i j = 1 }
						$print = { value = { \n } }
						$call = { id = 1 i = {$add = { $parameter.i 1 } } j = 1 }
					}
				}
			}

			Event =
			{
				id = x2
				$parameter = { i j }
				$option = { REMOVE_IF_CALL_ANY_EVENT }

				$if = {
					condition = { $COMP<EQ = { $parameter.j 9 } }
					then = {
						$print = { value = { $parameter.i } }
						$print = { value = { " * " } }
						$print = { value = { $parameter.j } }
						$print = { value = { " = " } }
						$print = { value = { $multiple = { $parameter.i $parameter.j } } }
						$print = { value = { \n } }
						$call = { id = x2 i = $parameter.i j = { $add = { $parameter.j 1 } } }
					}
				}
			}
		Main example
			Main =
			{
			    $call = { id = 0 }
			}

			
	ClauText support if, else statements. ( $if = { condition = {  } then = {  } } $else = { then = { } } )
	
	Now, ClauText support while directly.!!
	//ClauText does not support while directly. but support using if statements and Event Recursive Call.

# ClauText and Paradox Game ( EU4, DHR, ... )
	1. it can load eu4 savegames, and maybe other paradox game db or savegame files.
	2. it`s syntax come from eu4 savegame syntax.
 
# Example
	Main = { $call = { id = 0 } }

	Event = {
		id = 0

		$local = { i j }
		$assign = { $local.i value = { 1 } }

		$while = {
			condition = { 
				$COMP<EQ = { $local.i 9 } 
			}
			then = {
				$assign = { $local.j value = { 1 } }
				$while = {
					condition = {
						$COMP<EQ = { $local.j 9 }
					}
					then = {
						$if = { 
							condition = { $EQ = { 0 $modular = { $local.j 2 } } }
							then = {							
								$print = { data = { $local.i } }
								$print = { data = { " * " } }
								$print = { data = { $local.j } }
								$print = { data = { " = " } }
								$print = { data = { $multiple = { $local.i $local.j } } }
								$print = { enter = { \n } }
							}	
						}		
						$assign = { $local.j value = { $add = { 1 $local.j } } }		
					}
				}
				$print = { data = { ------- } }
				$print = { enter = { \n } }
				$assign = { $local.i value = { $add = { 1 $local.i } } }
			}
		}

		$_getch = { }
	}

