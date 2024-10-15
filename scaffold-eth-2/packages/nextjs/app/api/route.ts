import { NextRequest } from "next/server";
import { ethers } from "ethers";
import { CONTRACT_ABI, CONTRACT_ADDRESS, RPC_ENDPOINT, SIGNER_PRIVATE_KEY } from "~~/constants";

const provider = new ethers.JsonRpcProvider(RPC_ENDPOINT);
const contractAddress = CONTRACT_ADDRESS;
const contractABI = CONTRACT_ABI;

const contract = new ethers.Contract(contractAddress, contractABI, provider);

const formatNumber = (number: number) => Math.floor(number);

export async function GET() {
  try {
    const needsHeat = await contract.needsExtraHeat();

    return new Response(JSON.stringify({ needsHeat }), {
      status: 200,
      headers: { "Content-Type": "application/json" },
    });
  } catch (error) {
    return new Response(JSON.stringify({ error: "Error al leer el contrato" }), {
      status: 500,
      headers: { "Content-Type": "application/json" },
    });
  }
}

export async function POST(request: NextRequest) {
  try {
    const body = await request.json();
    console.log("Payload: ", body);
    const { temperature, humidity, pressure, airQuality } = body;

    const wallet = new ethers.Wallet(SIGNER_PRIVATE_KEY, provider);
    const contractWithSigner = contract.connect(wallet);

    const updateStatusMethod = contractWithSigner.getFunction("updateStatus");
    const tx = await updateStatusMethod(
      formatNumber(temperature),
      formatNumber(humidity),
      formatNumber(pressure),
      formatNumber(airQuality),
    );

    // Wait for transaction confirmation
    await tx.wait();

    return new Response(JSON.stringify({ txHash: tx.hash }), {
      status: 200,
      headers: { "Content-Type": "application/json" },
    });
  } catch (error) {
    return new Response(JSON.stringify({ error: "Error al actualizar el valor del contrato" }), {
      status: 500,
      headers: { "Content-Type": "application/json" },
    });
  }
}
