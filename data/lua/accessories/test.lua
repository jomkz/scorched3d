function runWeapon(playerId, position, velocity)

	print(testvar1);

	print(playerId);
	print(position.x, position.y, position.z);
	print(velocity.x, velocity.y, velocity.z);
	
	
	tanks = s3d.get_tanks();
	for k,v in pairs(tanks) do 
		if (v.alive) then 
			print(v.name);
			print(v.id);
			print(v.position.x, v.position.y, v.position.z);
			
			newvelocity = {x=0, y=0, z=0};
			
			s3d.explosion(playerId, v.position, { 
				size = 10,
				hurtamount = 1.0
			});
			
--			s3d.fire_weapon("Nuke", playerId, v.position, newvelocity);
		end
	end
end
