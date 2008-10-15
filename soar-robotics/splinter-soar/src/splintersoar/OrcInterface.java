package splintersoar;

import java.io.DataInputStream;
import java.io.IOException;

import laserloc.*;
import java.util.*;

import lcm.lcm.LCM;
import lcm.lcm.LCMSubscriber;
import lcmtypes.laser_t;
import lcmtypes.pose_t;

import orc.Motor;
import orc.Orc;
import orc.QuadratureEncoder;

import erp.geom.*;

public class OrcInterface implements LCMSubscriber
{
	Timer timer = new Timer();
	static final int UPDATE_HZ = 30;
	
	SplinterState state = new SplinterState();
	
	private Orc orc;
	private Motor [] motor = new Motor[2];
	private QuadratureEncoder [] odom = new QuadratureEncoder[2];
	
	private double [] command = { 0, 0 };
	
	pose_t pose;
	laser_t laserData;
	RangerData [] rangerData = new RangerData[ state.rangerSlices ];
	int [] previousMotorPosition = { 0, 0 };
	
	double [] initialPosition = new double[3];
	boolean haveInitialCoords = false;

	public OrcInterface()
	{
		Arrays.fill( initialPosition, 0 );
		
		orc = Orc.makeOrc();
		
		motor[0] = new Motor( orc, 1, true );
		odom[0] = new QuadratureEncoder( orc, 1, true);
		previousMotorPosition[0] = odom[0].getPosition();
		
		motor[1] = new Motor( orc, 0, false );
		odom[1] = new QuadratureEncoder( orc, 0, false);
		previousMotorPosition[1] = odom[1].getPosition();
		
		for ( int index = 0; index < rangerData.length; ++index)
		{
			rangerData[ index ] = new RangerData();
		}
		
		SplinterSoar.logger.info( "Orc up" );
		timer.schedule( new UpdateTask(), 0, 1000 / UPDATE_HZ );
	}
	
	public SplinterState getState()
	{
		return state;
	}
	
	public void shutdown()
	{
		timer.cancel();
		SplinterSoar.logger.info( "Orc down" );
	}

	boolean translating = false;
	double [] prevYawCalcLoc = new double[3]; 
	double yawCalcDistThreshold = 0.1;
	boolean hadSickData = false;
		
	class UpdateTask extends TimerTask
	{
		@Override
		public void run()
		{
			double left, right, targetYaw, targetYawTolerance, prevYaw;
			double [] previousPosition = new double[3];
			boolean targetYawEnabled;
			synchronized ( state )
			{
				left = state.left;
				right = state.right;
				targetYaw = state.targetYaw;
				targetYawTolerance = state.targetYawTolerance;
				targetYawEnabled = state.targetYawEnabled;
				
				System.arraycopy( state.pos, 0, previousPosition, 0, state.pos.length );
				prevYaw = state.yaw;
			}
			
			//// Communicate with orc
			// FIXME: each of the next get lines gets a new OrcStatus message. 
			// Really, we should only be getting one status message per update.
			// In the interest of not prematurely optimizing, I will leave this
			// like it is for now.
			int leftPosition = odom[0].getPosition();
			int rightPosition = odom[1].getPosition();

			// write new commands
			if ( targetYawEnabled == false )
			{
				motor[0].setPWM( left );
				motor[1].setPWM( right );
			} 
			//// end orc communication (more after yaw calculation)

			// calculation of new yaw is always based on odometry
			double dleft = ( leftPosition - previousMotorPosition[0] ) * state.tickMeters;
			double dright = ( rightPosition - previousMotorPosition[1] ) * state.tickMeters;
			double phi = ( dright - dleft ) / state.baselineMeters;
			double thetaprime = prevYaw + phi;

			// calculation of x,y
			double [] newPosition;
			if ( pose != null )
			{
				if ( haveInitialCoords == false )
				{
					System.arraycopy( pose.pos, 0, initialPosition, 0, pose.pos.length );
					haveInitialCoords = true;
				}
				
				System.out.print( "*" );
				newPosition = Geometry.subtract( pose.pos, initialPosition );
				
				hadSickData = true;
				pose = null;
			}
			else
			{
				System.out.print( "." );
				// Equations from A Primer on Odopmetry and Motor Control, Olson 2006
				// dleft, dright: distance wheel travelled
				// dbaseline: wheelbase
				//
				// dcenter = ( dleft + dright ) / 2
				// phi = ( dright - dleft ) / dbaseline
				// thetaprime = theta + phi
				// xprime = x + ( dcenter * cos( theta ) )
				// yprime = y + ( dcenter * sin( theta ) )
				
				double dcenter = ( dleft + dright ) / 2;
				
				newPosition = new double[3];
				newPosition[0] = previousPosition[0] + ( dcenter * Math.cos( prevYaw ) );
				newPosition[1] = previousPosition[1] + ( dcenter * Math.sin( prevYaw ) );
				newPosition[2] = 0;
			}
			
			// if we start moving forward or backward, mark that location
			// if we continue to move forward and move past a certain distance, recalculate yaw
			if ( hadSickData )
			{
				if ( translating )
				{
					if ( ( left >= 0 && right <= 0 ) || ( left <= 0 && right >= 0 ) )
					{
						translating = false;
						System.out.println( "Stopped translating" );
					}
				
					double [] deltaPos = Geometry.subtract( newPosition, prevYawCalcLoc );
					double distanceTravelled = Geometry.magnitude( deltaPos );
				
					if ( distanceTravelled > yawCalcDistThreshold )
					{
						double newThetaPrime = Math.atan2( deltaPos[1], deltaPos[0] );
						System.out.format( "Correcting yaw delta %.3f%n", Math.toDegrees( newThetaPrime ) - Math.toDegrees( thetaprime ) );
						thetaprime = newThetaPrime;
					
						System.arraycopy( newPosition, 0, prevYawCalcLoc, 0, newPosition.length ); 
						hadSickData = false;
					}
				}
				else
				{
					if ( ( left < 0 && right < 0 ) || ( left > 0 && right > 0 ) )
					{
						System.arraycopy( newPosition, 0, prevYawCalcLoc, 0, newPosition.length );
						translating = true;
						System.out.println( "Started translating" );
					}
				}
			}
			
			//// start orc communication
			if ( targetYawEnabled )
			{
				double relativeBearingValue = targetYaw - thetaprime;
				if ( relativeBearingValue > Math.PI )
				{
					relativeBearingValue -= 2 * Math.PI;
				} else if ( relativeBearingValue < Math.PI * -1 )
				{
					relativeBearingValue += 2 * Math.PI;
				}
				
				if ( relativeBearingValue < ( 0 - targetYawTolerance ) )
				{
					motor[0].setPWM( left );
					motor[1].setPWM( right * -1 );
				}
				else if ( relativeBearingValue > targetYawTolerance )
				{
					motor[0].setPWM( left * -1 );
					motor[1].setPWM( right );
				}
				else
				{
					motor[0].setPWM( 0 );
					motor[1].setPWM( 0 );
				}
			} 
			//// end orc communication

			previousMotorPosition[0] = leftPosition;
			previousMotorPosition[1] = rightPosition;

			//// ranger update
			if ( laserData != null )
			{
				// FIXME verify this is general, I think sliceChunk must have no remainder
				assert state.rangerSlices == 5;
				assert rangerData != null;
				assert rangerData.length == state.rangerSlices;
				
				int sliceChunk = laserData.nranges / state.rangerSlices; // a round number with 180/5 (36)
				
				for ( int slice = 0, index = 0; slice < rangerData.length; ++slice )
				{
					rangerData[ slice ].start = laserData.rad0 + index * laserData.radstep;

					rangerData[ slice ].distance = Double.MAX_VALUE;
					
					for ( ; index < ( sliceChunk * slice ) + sliceChunk; ++index )
					{
						if ( laserData.ranges[ index ] < rangerData[ slice ].distance )
						{
							rangerData[ slice ].distance = laserData.ranges[ index ];
						}
					}
					
					rangerData[ slice ].end = laserData.rad0 + ( index - 1 ) * laserData.radstep;
				}
			}
			//// end ranger update
			
			long utime = System.nanoTime() / 1000;			
			synchronized ( state )
			{
				state.utime = utime;
				
				state.leftPosition = leftPosition;
				state.rightPosition = rightPosition;
				
				if ( laserData != null )
				{
					state.ranger = Arrays.copyOf( rangerData, rangerData.length );
					state.rangerutime = utime;
				}
				
				System.arraycopy( newPosition, 0, state.pos, 0, newPosition.length );
				state.yaw = thetaprime;
			}

			laserData = null;
		}
	}
	
	@Override
	public void messageReceived( LCM lcm, String channel, DataInputStream ins ) 
	{
		if ( channel.equals( LaserLoc.pose_channel ) )
		{
			if ( pose != null )
			{
				return;
			}

			try 
			{
				pose = new pose_t( ins );
			} 
			catch ( IOException ex ) 
			{
				SplinterSoar.logger.warning( "Error decoding pose message: " + ex );
			}
		}
		else if ( channel.equals( "LASER_FRONT" ) )
		{
			if ( laserData != null )
			{
				return;
			}
			
			try 
			{
				laserData = new laser_t( ins );
			} 
			catch ( IOException ex ) 
			{
				SplinterSoar.logger.warning( "Error decoding LASER_FRONT message: " + ex );
			}
		}
	}

}
