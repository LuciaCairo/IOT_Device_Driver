//SPDX-License-Identifier: MIT
pragma solidity >=0.8.0 <0.9.0;

// Useful for debugging. Remove when deploying to a live network.
import "hardhat/console.sol";

// Use openzeppelin to inherit battle-tested implementations (ERC20, ERC721, etc)
// import "@openzeppelin/contracts/access/Ownable.sol";

/**
 * A smart contract that allows changing a state variable of the contract and tracking the changes
 * It also allows the owner to withdraw the Ether in the contract
 * @author BuidlGuidl
 */
contract YourContract {
	address public immutable owner;

	uint256 public lowerTempLimit = 24;
	uint256 public upperTempLimit = 30;
	bool public needsExtraHeat = false;

	uint256 public temperature = 0;
	uint256 public humidity = 0;
	uint256 public pressure = 0;
	uint256 public airQuality = 0;

	event HeatNeededChange(bool needsExtraHeat);

	// Constructor: Called once on contract deployment
	// Check packages/hardhat/deploy/00_deploy_your_contract.ts
	constructor(address _owner) {
		owner = _owner;
	}

	// Modifier: used to define a set of rules that must be met before or after a function is executed
	// Check the withdraw() function
	modifier isOwner() {
		// msg.sender: predefined variable that represents address of the account that called the current function
		require(msg.sender == owner, "Not the Owner");
		_;
	}

	function setLowerTempLimit(uint256 _lowerTempLimit) public {
		lowerTempLimit = _lowerTempLimit;
	}

	function setUpperTempLimit(uint256 _upperTempLimit) public {
		upperTempLimit = _upperTempLimit;
	}

	/**
	 * Function to update the status of the contract based on the environment data.
	 */
	function updateStatus(
		uint256 _temperature,
		uint256 _humidity,
		uint256 _pressure,
		uint256 _airQuality
	) public {
		// Change state variables
		temperature = _temperature;
		humidity = _humidity;
		pressure = _pressure;
		airQuality = _airQuality;

		if (temperature <= lowerTempLimit) {
			needsExtraHeat = true;
		} else if (temperature >= upperTempLimit) {
			needsExtraHeat = false;
		}

		// Trigger the event
		emit HeatNeededChange(needsExtraHeat);
	}
}
